#ifndef PLAYER_H
#define PLAYER_H

#include "definition.h"
#include "object.h"
#include <vector>
#include <set>
namespace DATA
{
	struct Data;
}

class Student;

class Player :public Object<Player>
{
private:
	std::set<TId> m_students;         //所属同学
	TDepartment m_department;            //院系种类
	TResourceD m_technologyPoint = 0.0;        //科技点数

	TLevel m_RegenerationSpeedLevel = 0;      //再生倍率等级
	TLevel m_ExtendingSpeedLevel = 0;      //延伸速度等级
	TLevel m_ExtraControlLevel = 0;        //额外操作数等级
	TLevel m_DefenceLevel = 0;       //防御等级

	TRound m_skillLeftRound = 0;       //剩余主动技能回合
	std::size_t m_maxControlNumber;    //最大控制数

	DATA::Data* const  data;

	TRound m_hackedLeftRound = 0;                //是否被黑
	bool m_alive = true;                  //是否还活着

	//升级各种被动属性，返回是否成功
	bool _upgradeRegeneration();
	bool _upgradeExtending();
	bool _upgradeExtraControl();
	bool _upgradeDefence();

public:
	Player():Object(NO_DATA),data(nullptr){}
	Player(DATA::Data* _data, TDepartment _department, std::set<TId> _stu);

	void addStudent(TStudentID student);             //增加同学
	void removeStudent(TStudentID student);          //减少同学
	void setHacked(TRound HachRound);
	bool upgrade(TPlayerPowerProperty property);    //升级    
	void addTechPoint();                      //根据当前细胞增加科技点数
	void Kill();                              //杀死该玩家
	void reduceTech(TResourceD minus) { m_technologyPoint -= minus; }
	void updateMaxControl() { m_maxControlNumber = m_students.size() / 2 + 1; }

	//得到各种量

	TRound HackedLeftRound() { return m_hackedLeftRound; }
	bool isAlive() { return m_alive; }
	TResourceD techPoint() { return m_technologyPoint; }
	std::set<TId> getStudents() { return m_students; }
	std::size_t getStudentNumber() { return m_students.size(); }
	std::size_t maxControlNumber() { return m_maxControlNumber;}
	TDepartment getDepartment() { return m_department; }
	TLevel regenerationSpeedLevel() { return m_RegenerationSpeedLevel; }
	TLevel extendingSpeedLevel() { return m_ExtendingSpeedLevel; }
	TLevel extraControlLevel() { return m_ExtraControlLevel; }
	TLevel defenceLevel() { return m_DefenceLevel; }
	void   setSkillLeftRound(TRound round) { m_skillLeftRound = round; }
	void   setMaxControl(int max) { m_maxControlNumber = max; }
	void   setHackedRound(TRound round) { m_hackedLeftRound = round; }
	TRound skillLeftRound() { return m_skillLeftRound; }
	TResourceD totalResource();
};

#endif // PLAYER_H
