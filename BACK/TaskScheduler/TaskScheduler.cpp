#include <SDK/TaskScheduler/TaskScheduler.h>
#include <SDK/Offset.h>
RBX::TaskScheduler::TaskScheduler() {
	Address = GetScheduler();
}

u64 RBX::TaskScheduler::GetScheduler() {
	return comms->read<u64>(comms->image_address +Offsets::JobsPointer);
}

std::vector<u64> RBX::TaskScheduler::GetActiveJobs() {
	std::vector<u64> Jobs;

	u64 Count = comms->read<u64>(Address - sizeof(u64));
	for (u64 i = 0; i <= Count;i += sizeof(std::shared_ptr<void*>)) {
		u64 Job = comms->read<u64>(Address + i);
		if (Job == 0) {
			break;
		}
		Jobs.push_back(Job);
	}

	return Jobs;
}

u64 RBX::TaskScheduler::GetJob(const str& name) {
	for (u64& Job : GetActiveJobs()) {
		if (GetJobName(Job) == name) {
			return Job;
		}
	}
	return 0;
}

str RBX::TaskScheduler::GetJobName(u64 job) {
	return comms->readstr(job + Offsets::JobName);
}
u64 RBX::TaskScheduler::GetRenderView() {
	u64 RenderJob = GetJob("RenderJob");

	return comms->read<u64>(RenderJob + Offsets::RenderViewPtr);

}
u64 RBX::TaskScheduler::GetDataModel() {
	u64 RenderJob = GetJob("RenderJob");
	

	return comms->read<u64>(RenderJob + Offsets::DataModelPtr); +Offsets::DataModelOffset;
}
u64 RBX::TaskScheduler::GetVisualEngine() {
	u64 RenderView = GetRenderView();


	return comms->read<u64>(RenderView + Offsets::VisualEnginePtr);
}
void RBX::TaskScheduler::PrintJobs() {
	for (u64& Job : GetActiveJobs()) {
		printf("[?] Task Scheduler: Found Job [0x%llx - %s]\n", Job, GetJobName(Job).c_str());
	}
}

bool RBX::TaskScheduler::IsLoaded() {
	return GetActiveJobs().size() != 0;
}