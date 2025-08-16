#pragma once

namespace RBX {
	class TaskScheduler {
	public:
		u64 Address;

		TaskScheduler();

		u64 GetScheduler();
	private:
		str GetJobName(u64 job);
		u64 GetJob(const str& name);

		std::vector<u64> GetActiveJobs();
		
	public:
		u64 GetVisualEngine();
		u64 GetRenderView();
		u64 GetDataModel();

		void PrintJobs();
		bool IsLoaded();

	
	};
}