/******************************************************************************
    Copyright (C) 2023 by Lain Bailey <lain@obsproject.com>
                          Philippe Groarke <philippe.groarke@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <obs.hpp>
#include <util/util.hpp>
#include <util/lexer.h>
#include <graphics/math-defs.h>
#include <initializer_list>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <QCompleter>
#include <QGuiApplication>
#include <QLineEdit>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDirIterator>
#include <QVariant>
#include <QTreeView>
#include <QScreen>
#include <QStandardItemModel>
#include <QSpacerItem>
#include <qt-wrappers.hpp>

#include "audio-encoders.hpp"
#include "hotkey-edit.hpp"
#include "source-label.hpp"
#include "obs-app.hpp"
#include "platform.hpp"
#include "properties-view.hpp"
#include "window-basic-main.hpp"
#include "window-basic-settings.hpp"

#include <util/platform.h>
#include <util/dstr.hpp>
#include "ui-config.h"

using namespace std;


QWidget* OBSBasicSettings::getAdvancedPageWidget() {
	return ui->widget_11;
}

QWidget* OBSBasicSettings::getHotkeyPageWidget() {
    LoadHotkeySettings();
	return ui->hotkeyScrollArea;
}


QSpinBox* OBSBasicSettings::getSimpleOutputVBitrate() {
	return ui->simpleOutputVBitrate;
}

QComboBox* OBSBasicSettings::getSimpleOutStrEncoder() {
    return ui->simpleOutStrEncoder;
}
QComboBox* OBSBasicSettings::getSimpleOutputABitrate() {
    return ui->simpleOutputABitrate;
}

QComboBox* OBSBasicSettings::getSimpleOutPreset() {
        return ui->simpleOutPreset;
}

QComboBox* OBSBasicSettings::getSimpleOutStrAEncoder() {
    return ui->simpleOutStrAEncoder;
}

QComboBox* OBSBasicSettings::getSimpleOutRecQuality() {
    return ui->simpleOutRecQuality;
}

QComboBox* OBSBasicSettings::getSimpleOutRecFormat() {
        return ui->simpleOutRecFormat;
}



QComboBox* OBSBasicSettings::getBaseResolution() {
    return ui->baseResolution;
}

QComboBox* OBSBasicSettings::getOutputResolution() {
    return ui->outputResolution;
}

QComboBox* OBSBasicSettings::getDownscaleFilter() {
        return ui->downscaleFilter;
}
QComboBox *OBSBasicSettings::getFpsType() {
    return ui->fpsType;
}
QStackedWidget* OBSBasicSettings::getFpsTypes() {
        return ui->fpsTypes;
}


QLabel* OBSBasicSettings::getBaseAspect() {
    return ui->baseAspect;
}
QLabel* OBSBasicSettings::getScaledAspect() {
    return ui->scaledAspect;
}


void OBSBasicSettings::onCofirm()
{
		SaveSettings();

		//UpdateYouTubeAppDockSettings();
		ClearChanged();
}


void OBSBasicSettings::onCancel() {
		ClearChanged();
		//close();
}


void OBSBasicSettings::onApply() {
    SaveSettings();
}