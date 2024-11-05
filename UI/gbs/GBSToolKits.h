#ifndef __GBS_TOOLKTIS_H__
#define __GBS_TOOLKTIS_H__
#include <memory>
#include <string>

#include <QString>
#include <list>
#include "obs.h"
#include "obs.hpp"

using GBSOjbect = obs_object_t;
using GBDisplay = obs_display_t;
using GBSView = obs_view_t;
using GBSSource = obs_source_t;
using GBSScent = obs_scene_t;
using GBSScentItem = obs_sceneitem_t;
using GBSOutpu = obs_output_t;
using GBSEncoder = obs_encoder_t;
using GBSEncoderGroup = obs_encoder_group_t;
using GBSService = obs_service_t;
using GBSModule = obs_module_t;
using GBSFader = obs_fader_t;
using GBSVolmeter = obs_volmeter_t;

using GBSWeakObject = obs_weak_object_t;
using GBSWeakSource = obs_weak_source_t;
using GBSWeakOutput = obs_weak_output_t;
using GBSWeakEncoder = obs_weak_encoder_t;
using GBSWeakService = obs_weak_service_t;

class GBSToolKits {


public:
	virtual ~GBSToolKits() {}
	static std::shared_ptr<GBSToolKits> getInstance()
	{
		static std::shared_ptr<GBSToolKits> instance(new GBSToolKits());
		return instance;
	}


	GBSSource *CreateSource(std::string id, std::string name);
	void DestroySource(GBSSource *source);
	std::list<QString> EnumDevice(GBSSource *source);
	std::list<QString> EnumAudioDevice(GBSSource *source);


	void StartPreview(GBSSource *source, int index);
	
	void StopPreview(GBSSource *source);


	void StartRtmpPush(std::string rtmpUrl, GBSSource *source);

	void list_audio_devices();
	void list_video_devices();
	


private:
	GBSToolKits();

//private:
//	OBSService service;
//	std::unique_ptr<BasicOutputHandler> outputHandler;
//	std::shared_future<void> setupStreamingGuard;

};



#endif //__GBS_TOOLKTIS_H__
