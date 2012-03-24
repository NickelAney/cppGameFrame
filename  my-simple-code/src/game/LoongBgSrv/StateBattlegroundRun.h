/*
 * StateBattlegroundRun.h
 *
 *  Created on: 2012-3-23
 *    
 */

#ifndef GAME_STATEBATTLEGROUNDRUN_H_
#define GAME_STATEBATTLEGROUNDRUN_H_

#include <mysdk/base/Common.h>

#include <game/LoongBgSrv/BattlegroundState.h>

class StateBattlegroundRun:public BattlegroundState
{
public:
	StateBattlegroundRun(Battleground* bg);
	virtual ~StateBattlegroundRun();

	virtual void start();
	virtual void run(uint32 curTime);
	virtual void end();

	virtual BgStateE getState() { return BGSTATE_RUN; }
	virtual uint32	getStateTimeLimit() { return 600; }
	virtual std::string getStateName() { return "激战中阶段"; }

private:

};

#endif /* STATEBATTLEGROUNDRUN_H_ */