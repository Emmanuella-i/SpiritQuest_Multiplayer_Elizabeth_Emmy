#pragma once
//ET
enum class MissionStatus
{
	kMissionRunning,
	kMissionSuccessGhostFL,// ghost win finish line
	kMissionSuccessGhostRD,// ghost win reaper Dies
	kMissionFailureReaper, //reaper passes finish line before ghost 
	kMissionSuccessReaperCG,// reaper win, catches ghost
	kMissionSuccessReaperGD,// reper win, ghost dies
	kMissionFailure // both die
};