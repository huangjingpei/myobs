#include "../win-update/updater/manifest.hpp"
#include "update-helpers.hpp"
#include "shared-update.hpp"
#include "update-window.hpp"
#include "remote-text.hpp"
#include "win-update-gbs.hpp"
#include "obs-app.hpp"

#include <qt-wrappers.hpp>
#include <QMessageBox>


#include <string>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

#include <util/windows/WinHandle.hpp>
#include <util/util.hpp>

#ifdef BROWSER_AVAILABLE
#include <browser-panel.hpp>
#endif

#include "gbs/updater/GBSXMLParser.h"
#include "gbs/updater/GBSFileDownloader.h"
#include "gbs/updater/winhttp/TLBufferVector.h"
#include "gbs/updater/winhttp/TLDownloadTask.h"
#include "gbs/updater/winhttp/TLWebDef.h"
#include "gbs/updater/winhttp/TLWinHttpDownloader.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/common/QBizLogger.h"

using namespace std;
using namespace updater;

/* ------------------------------------------------------------------------ */
#if 1
#ifndef WIN_MANIFEST_URL
#define WIN_MANIFEST_URL "http://121.43.150.109/obs-updater/manifest.json"
#endif

#ifndef WIN_MANIFEST_BASE_URL
#define WIN_MANIFEST_BASE_URL "http://121.43.150.109/obs-updater/"
#endif

#ifndef WIN_BRANCHES_URL
#define WIN_BRANCHES_URL "http://121.43.150.109/obs-updater/branches.json"
#endif

#ifndef WIN_DEFAULT_BRANCH
#define WIN_DEFAULT_BRANCH "stable"
#endif

#ifndef WIN_UPDATER_URL
#define WIN_UPDATER_URL "http://121.43.150.109/obs-updater/updater.exe"
#endif
#else
#ifndef WIN_MANIFEST_URL
#define WIN_MANIFEST_URL "https://obsproject.com/update_studio/manifest.json"
#endif

#ifndef WIN_MANIFEST_BASE_URL
#define WIN_MANIFEST_BASE_URL "https://obsproject.com/update_studio/"
#endif

#ifndef WIN_BRANCHES_URL
#define WIN_BRANCHES_URL "https://obsproject.com/update_studio/branches.json"
#endif

#ifndef WIN_DEFAULT_BRANCH
#define WIN_DEFAULT_BRANCH "stable"
#endif

#ifndef WIN_UPDATER_URL
#define WIN_UPDATER_URL "https://obsproject.com/update_studio/updater.exe"
#endif

#endif

/* ------------------------------------------------------------------------ */

static bool ParseUpdateManifest(const char *manifest_data, bool *updatesAvailable, string &notes, string &updateVer,
				const string &branch)
try {
	constexpr uint64_t currentVersion = (uint64_t)LIBOBS_API_VER << 16ULL | OBS_RELEASE_CANDIDATE << 8ULL |
					    OBS_BETA;
	constexpr bool isPreRelease = false;

	json manifestContents = json::parse(manifest_data);
	Manifest manifest = manifestContents.get<Manifest>();

	if (manifest.version_major == 0 && manifest.commit.empty())
		throw strprintf("Invalid version number: %d.%d.%d", manifest.version_major, manifest.version_minor,
				manifest.version_patch);

	notes = manifest.notes;

	if (manifest.commit.empty()) {
		uint64_t new_ver = MAKE_SEMANTIC_VERSION((uint64_t)manifest.version_major,
							 (uint64_t)manifest.version_minor,
							 (uint64_t)manifest.version_patch);
		new_ver <<= 16;
		/* RC builds are shifted so that rc1 and beta1 versions do not result
		 * in the same new_ver. */
		if (manifest.rc > 0)
			new_ver |= (uint64_t)manifest.rc << 8;
		else if (manifest.beta > 0)
			new_ver |= (uint64_t)manifest.beta;

		updateVer = to_string(new_ver);

		/* When using a pre-release build or non-default branch we only check if
		 * the manifest version is different, so that it can be rolled back. */
		if (branch != WIN_DEFAULT_BRANCH || isPreRelease)
			*updatesAvailable = new_ver != currentVersion;
		else
			*updatesAvailable = new_ver > currentVersion;
	} else {
		/* Test or nightly builds may not have a (valid) version number,
		 * so compare commit hashes instead. */
		updateVer = manifest.commit.substr(0, 8);
		*updatesAvailable = true;
	}

	return true;

} catch (string &text) {
	blog(LOG_WARNING, "%s: %s", __FUNCTION__, text.c_str());
	return false;
}

/* ------------------------------------------------------------------------ */

bool GetBranchAndUrlV2(string &selectedBranch, string &manifestUrl)
{
	const char *config_branch = config_get_string(App()->GetAppConfig(), "General", "UpdateBranch");
	if (!config_branch)
		return true;

	bool found = false;
	for (const UpdateBranch &branch : App()->GetBranches()) {
		if (branch.name != config_branch)
			continue;
		/* A branch that is found but disabled will just silently fall back to
		 * the default. But if the branch was removed entirely, the user should
		 * be warned, so leave this false *only* if the branch was removed. */
		found = true;

		if (branch.is_enabled) {
			selectedBranch = branch.name.toStdString();
			if (branch.name != WIN_DEFAULT_BRANCH) {
				manifestUrl = WIN_MANIFEST_BASE_URL;
				manifestUrl += "manifest_" + branch.name.toStdString() + ".json";
			}
		}
		break;
	}

	return found;
}

/* ------------------------------------------------------------------------ */

void GBSAutoUpdateThread::infoMsg(const QString &title, const QString &text)
{
	OBSMessageBox::information(App()->GetMainWindow(), title, text);
}

void GBSAutoUpdateThread::info(const QString &title, const QString &text)
{
	QMetaObject::invokeMethod(this, "infoMsg", Qt::BlockingQueuedConnection, Q_ARG(QString, title),
				  Q_ARG(QString, text));
}

int GBSAutoUpdateThread::queryUpdateSlot(bool localManualUpdate, const QString &text)
{
	OBSUpdate updateDlg(App()->GetMainWindow(), localManualUpdate, text);
	return updateDlg.exec();
}

int GBSAutoUpdateThread::queryUpdate(bool localManualUpdate, const char *text_utf8)
{
	int ret = OBSUpdate::No;
	QString text = text_utf8;
	QMetaObject::invokeMethod(this, "queryUpdateSlot", Qt::BlockingQueuedConnection, Q_RETURN_ARG(int, ret),
				  Q_ARG(bool, localManualUpdate), Q_ARG(QString, text));
	return ret;
}


bool GBSAutoUpdateThread::queryRepairSlot()
{
	QMessageBox::StandardButton res =
		OBSMessageBox::question(App()->GetMainWindow(), QTStr("Updater.RepairConfirm.Title"),
					QTStr("Updater.RepairConfirm.Text"), QMessageBox::Yes | QMessageBox::Cancel);

	return res == QMessageBox::Yes;
}

void GBSAutoUpdateThread::onDownloadPercentage(int32_t percentage) {
	if (percentage % 10 == 0) {
		QLogE("update: percentage %d\n", percentage);
	}
	qDebug() << "update percent :" << percentage;
}

void GBSAutoUpdateThread::onDownloadBytes(uint32_t downloadBytes, uint32_t allBytes) {
	
}

bool GBSAutoUpdateThread::queryRepair()
{
	bool ret = false;
	QMetaObject::invokeMethod(this, "queryRepairSlot", Qt::BlockingQueuedConnection, Q_RETURN_ARG(bool, ret));
	return ret;
}

void GBSAutoUpdateThread::run() {
	try {
		string text;
		string branch = WIN_DEFAULT_BRANCH;
		string manifestUrl = WIN_MANIFEST_URL;
		vector<string> extraHeaders;
		bool updatesAvailable = false;

		struct FinishedTrigger {
			inline ~FinishedTrigger()
			{
				QMetaObject::invokeMethod(App()->GetMainWindow(), "updateCheckFinished");
			}
		} finishedTrigger;

		QString newFile = QCoreApplication::applicationDirPath() + "/appcast.xml";
		TLDownloadTask task;
		task.m_strUrl = "https://renew.guobo.shop/exe/gbpc/appcast.xml";
		task.m_strAgent = "userClient";
		task.m_uReadBytes = 0;
		TLWinHttpDownloader down;
		down.setEventHandler(this);
		down.DownloadToFile(task, newFile.toStdString().c_str());

		GBSXMLParser xmlParser;
		xmlParser.parseXml(newFile);
		QList<Enclosure> enclosures = xmlParser.getEnclosures();
		QString updateUrl = "";
		QString version = "";
		for (auto enclosure : enclosures) {
			updateUrl = enclosure.url;
			version = enclosure.version;
			break;
		}
		QList<QString> features = xmlParser.getFeatures();
		features.prepend("更新说明:" );
		features.prepend("版本号:" + version);


		std::string curVersion = GBSMainCollector::getInstance()->getSoftWareVersion();
		QString qCurVersion = QString::fromStdString(curVersion);

		if (qCurVersion.compare(version) >= 0) {
			return;
		}
		if (manualUpdate) {
			emit sigHasNewerVersion(features);
			return;
		}
		QLogE("update: current version: %s new version:%s\n", qCurVersion.toStdString().c_str(),
		      version.toStdString().c_str());
		QStringList parts = updateUrl.split("/");
		QString fileNmae = parts.last();
		QString exeName = QCoreApplication::applicationDirPath() + "/" + fileNmae;

		{
			TLDownloadTask task;
			task.m_strUrl = updateUrl.toStdString().c_str();
			task.m_strAgent = "userClient";
			task.m_uReadBytes = 0;
			TLWinHttpDownloader down;
			down.setEventHandler(this);
			down.DownloadToFile(task, exeName.toStdString().c_str());
		}
		emit sigDownloadFinished();
		return;
	} catch (string &text) {
		blog(LOG_WARNING, "%s: %s", __FUNCTION__, text.c_str());
	}
}

