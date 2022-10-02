#include <Windows.h>
#include "conio.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <atlbase.h>
#include <atlstr.h>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <map>
#include <cctype>
#include <bitset>
/*
dgncx(c) header version.
(ONLY FOR THE HEADER VERSION (dgncx_h) !!!)
change-user: <williamwei>
please put this and dgncx_h_v1.0.1.cpp together.
thank you.
41105
*/
#undef MessageBox
#define MessageBox MessageBoxW
using namespace std;
const int maxn = 100001;
int a[maxn], b[maxn], c[maxn], i;
char n[maxn], n1[maxn], n2[maxn];
char a1[maxn], b1[maxn];
constexpr int PLAYERCOUNT = 3;
constexpr int CARDSCOUNT = 54;
constexpr int CURRENTPLAYER = 0;
constexpr int VALUECOUNT = 17;
#undef ERROR
// error is a C-standard definition. undef this to define <ERROR>
#define SZ(a) ((int)a.size())
constexpr int ERROR = -1;
#define debug(...) 42
using namespace std;
#define I cin
#define O cout
#define rep(i,a,n) for (int i=a;i<n;++i)
int n_puzzle_3[3][3]{ {1,2,3},{4,5,6},{7,8,0} };
int n_puzzle[3][3]{ {1,3,8},{7,0,2},{5,6,4} };
typedef stringstream ss;
#pragma warning(disable:4996)
inline LPCWSTR strtolp(const char* czStr) { CString str = CString(czStr); USES_CONVERSION; LPCWSTR wszClassName = new WCHAR[str.GetLength() + 1]; wcscpy((wchar_t*)wszClassName, T2W((LPTSTR)str.GetBuffer(NULL))); str.ReleaseBuffer(); return wszClassName; }
void inttostr(int a, string& b) { ss s; s << a; b = s.str(); }
int gcd(int a, int b) { // 最强gcd, 不容反驳
	int ret = 1;
	while (a != 0) {
		if ((~a & 1) && (~b & 1)) ret <<= 1, a >>= 1, b >>= 1;
		else if (~a & 1) a >>= 1; else if (~b & 1) b >>= 1;
		else {
			if (a < b) swap(a, b);
			a -= b;
		}
	}
	return ret * b;
}
struct frac {
	int n, de;//分子 分母
	bool operator>(frac rhs) const { return n * rhs.de > rhs.n * de; }
	bool operator<(frac rhs) const { return n * rhs.de < rhs.n* de; }
	inline void operator=(frac& rhs) { n = rhs.n; de = rhs.de; }
	inline void operator+(frac& rhs) { int a = de * rhs.de / gcd(rhs.de, de); n = (rhs.n * (a / rhs.de) + n * (a / de)); de = a; int gc = gcd(n, de); n /= gc; de /= gc; }
	inline void operator-(frac& rhs) {
		if (*this > rhs) { int a = de * rhs.de / gcd(rhs.de, de); n = (n * (a / de) - rhs.n * (a / rhs.de)); de = a; }
		else { int a = de * rhs.de / gcd(rhs.de, de); n = (rhs.n * (a / rhs.de) - n * (a / de)); de = a; }
		int gc = gcd(n, de); n /= gc; de /= gc;
	}
	inline void operator*(frac rhs) { n *= rhs.n; de *= rhs.de; int gc = gcd(n, de); n /= gc; de /= gc; }
	inline void operator/(frac rhs) { n *= rhs.de; de *= rhs.n; int gc = gcd(n, de); n /= gc; de /= gc; }
	inline void operator<<(frac rhs) { n *= (1 << rhs.n); de *= (1 << rhs.de); }
	inline void operator>>(frac rhs) { n /= (1 >> rhs.n); de /= (1 >> rhs.de); }
	// 模板一笔带过, 但非常有用[doge]
};
void outArr() {
	rep(i, 0, 3) {
		rep(j, 0, 3) {
			if (n_puzzle[i][j] != 0) O << n_puzzle[i][j] << " ";
			else O << "  ";
		}
		O << '\n';
	}
}
template<typename T>
inline void change(T one, T two) { swap(one, two); }
int scnt = 0;
const char toFigure[] = "34567890JQKA 2YZ";
enum COLOR {  //花色显示ASCII: 3~6
	eHEART = 3,//红桃 
	eDIAMOND,//方片 
	eCLUB,   //草花 
	eSPADE   //黑桃 
};
class Card; class CardsType; class CardGroup; class Player; class Landlords; class LastCards;
bool makeChoice(string tip); bool cmp(Card* a, Card* b);
class Card {
public:
	char figure;
	COLOR color;
	int value;
	Card(char _figure, COLOR _color) { figure = _figure; color = _color; value = calValue(); }
	int calValue() {
		for (int i = 0; toFigure[i]; i++)
			if (toFigure[i] == figure)
				return i;
		return ERROR;
	}
	void print() {
		if (figure == 'Z') O << "Z";
		else if (figure == 'Y') O << "Y";
		else O << figure << static_cast<char>(color);
		O << ' ';
	}
};
class CardsType {
public:
	//为了规范查找对应牌的方法
	//统一为3个参数cnt1、isContinuous、cnt2
	int typeId;
	string typeStr;
	int cnt1 = 0, cnt2 = 0;
	bool isContinuous = false;
	CardsType() { typeId = ERROR; }
	bool operator ==(const CardsType& other)const { return this->typeId == other.typeId; }
	void init(string _typeStr, int _typeId, int _cnt1, bool _isContinuous, int _cnt2) {
		cnt1 = _cnt1;
		isContinuous = _isContinuous;
		cnt2 = _cnt2;
		typeStr = _typeStr;
		typeId = _typeId;
	}
};
class CardGroup {
public:
	vector<Card*> cards;
	CardsType type;
	void calType() {
		int i = 0, n = SZ(cards);
		if (n == 0) {
			type.init("不出", 14, 0, 0, 0);
			return;
		}
		if (n == 2 && cards[0]->value == 15 && cards[1]->value == 14) {
			type.init("王炸", 0, 0, 0, 0);
			return;
		}
		int cntFlag[VALUECOUNT]{};
		rep(i, 0, n) cntFlag[cards[i]->value]++;
		int maxCnt = 0, minCnt = 4;
		rep(i, 0, VALUECOUNT) {
			if (maxCnt < cntFlag[i]) maxCnt = cntFlag[i];
			if (cntFlag[i] && minCnt > cntFlag[i]) minCnt = cntFlag[i];
		}
		{
			if (n == 4 && maxCnt == 4) {
				type.init("炸弹", 1, 4, 0, 0);
				return;
			}
			else if (n == 1) {
				type.init("单牌", 2, 1, 0, 0);
				return;
			}
			else if (n == 2 && maxCnt == 2) {
				type.init("对子", 3, 2, 0, 0);
				return;
			}
			else if (n == 3 && maxCnt == 3) {
				type.init("三张 ", 4, 3, 0, 0);
				return;
			}
			else if (n == 4 && maxCnt == 3) {
				type.init("三带一", 5, 3, 0, 1);
				return;
			}
			else if (n == 5 && maxCnt == 3 && minCnt == 2) {
				type.init("三带一对", 6, 3, 0, 2);
				return;
			}
			else if (n == 6 && maxCnt == 4) {
				type.init("四带二", 7, 4, 0, 1);
				return;
			}
			else if (n == 8 && maxCnt == 4 && minCnt == 2) {
				type.init("四带二", 8, 4, 0, 2);
				return;
			}
			else if (n >= 5 && maxCnt == 1 && cards[0]->value == cards[n - 1]->value + n - 1) {
				type.init("顺子", 9, 1, 1, 0);
				return;
			}
			else if (n >= 6 && maxCnt == 2 && minCnt == 2 && cards[0]->value == cards[n - 1]->value + n / 2 - 1) {
				type.init("连对", 10, 2, 1, 0);
				return;
			}
			int fjCnt = 0;//统计连续且大于3三张的牌 
			for (; i < VALUECOUNT && cntFlag[i] < 3; i++);
			for (; i < VALUECOUNT && cntFlag[i] >= 3; i++, fjCnt++);
			if (fjCnt > 1)
				if (n == fjCnt * 3) type.init("飞机", 11, 3, 1, 0);
				else if (n == fjCnt * 4) type.init("飞机", 12, 3, 1, 1);
				else if (n == fjCnt * 5 && minCnt == 2) type.init("飞机", 13, 3, 1, 2);
		}
	}
	void init(string inputStr, vector<Card*>& cardsHolded) {
		this->cards.clear();
		//不出 
		if (inputStr == "N") {
			this->calType();
			return;
		}
		//int i, j;  // 为什么要这么写呢?容易串啊
		//输入合法性判断 
		{ // 模块化限制
			rep(i, 0, SZ(inputStr)) {
				bool find = false;
				for (int j = 0; toFigure[j]; j++)
					if (inputStr[i] == toFigure[j]) {
						find = true;
						break;
					}
				//输入字符不在toFigure中
				if (find == false) return;
			}
			//查找手中有没有这些牌 
			int visitFlag[20] = { 0 };
			rep(i, 0, SZ(inputStr)) {
				Card* find = NULL;
				rep(j, 0, SZ(cardsHolded))
					if (!visitFlag[j] && cardsHolded[j]->figure == inputStr[i]) {
						visitFlag[j] = 1;
						find = cardsHolded[j];
						break;
					}
				if (find) this->cards.push_back(find);
				else {
					O << inputStr[i];
					O << "没有找到\t";
					this->cards.clear();
					return;
				}
			}
			this->arrange();
		}
	}
	void init(vector<Card*> newCards) {
		this->cards = newCards;
		this->arrange();
	}
	bool isCanBeat(CardGroup& cardGroup) {
		if (cardGroup.type.typeStr == "王炸")
			return false;
		else if (this->type.typeStr == "王炸")
			return true;
		else if (cardGroup.type == this->type && this->type.typeStr == "炸弹")
			return value() > cardGroup.value();
		else if (cardGroup.type.typeStr == "炸弹")
			return false;
		else if (this->type.typeStr == "炸弹")
			return true;
		else if (cardGroup.type == this->type && SZ(this->cards) == SZ(cardGroup.cards))
			return this->value() > cardGroup.value();
		else return false;
	}
	int value() {
		if (type.typeStr == "三带一" || type.typeStr == "三带一对" || type.typeStr == "飞机") rep(i, 2, SZ(cards)) if (cards[i]->value == cards[i - 2]->value) return cards[i]->value;
		if (type.typeStr == "四带二")
			rep(i, 3, SZ(cards)) if (cards[i]->value == cards[i - 3]->value) return cards[i]->value;
		return cards[0]->value;
	}
	void arrange() { //整理：排序、计算类型 
		stable_sort(this->cards.begin(), this->cards.end(), cmp);
		this->calType();
	}
};
class LastCards {
	static LastCards* lastCards;
public:
	Player* player = NULL;
	CardGroup cardGroup;
	static LastCards* inst() {//单例模式 
		if (lastCards == NULL)
			lastCards = new LastCards();
		return lastCards;
	}
	vector<Card*> findCanBeatFrom(vector<Card*>& cardsHolded) {
		//查找能打得过的牌 
		int j, k, n = SZ(cardsHolded), m = SZ(cardGroup.cards);
		string typeStr = cardGroup.type.typeStr;
		vector<Card*> ret;
		//打不过，返回空数组
		if (typeStr == "王炸" || n < m)return ret;
		{
			int value = cardGroup.value();
			//统计各点牌出现的次数 
			int cntFlag[VALUECOUNT] = { 0 };
			rep(i, 0, n)
				cntFlag[cardsHolded[i]->value]++;
			int continuousCount = 1;
			if (cardGroup.type.isContinuous) {
				continuousCount = m / (cardGroup.type.cnt1 + cardGroup.type.cnt2);
			}
			bool findFirstFigure;
			rep(i, value + 1, VALUECOUNT) {
				findFirstFigure = true;
				for (j = 0; j < continuousCount; j++) {
					if (cntFlag[i - j] < cardGroup.type.cnt1) {
						findFirstFigure = false;
						break;
					}
				}
				if (findFirstFigure) {
					ret.clear();
					int firstFigure = i;
					for (k = 0, j = 0; k < SZ(cardsHolded) && j < continuousCount; k++) {
						if (cardsHolded[k]->value == firstFigure - j) {
							for (int kk = 0; j >= 0 && kk < cardGroup.type.cnt1; kk++)
								ret.push_back(cardsHolded[k + kk]);
							j++;
						}
					}
					if (cardGroup.type.cnt2 > 0) {
						int SecondFigures[5]{};
						int SecondCount = continuousCount;
						if (cardGroup.type.typeStr == "四带二")
							SecondCount = 2;
						bool findSecondFigure = true;
						for (j = 0, k = -1; j < SecondCount && findSecondFigure; j++) {
							findSecondFigure = false;
							for (k++; k < VALUECOUNT; k++) {
								SecondFigures[j] = k;
								if (cntFlag[k] >= cardGroup.type.cnt2 && cntFlag[k] < cardGroup.type.cnt1) {
									findSecondFigure = true;
									break;
								}
							}
						}
						if (findSecondFigure) {
							rep(ii, 0, SecondCount) {
								rep(jj, 0, SZ(cardsHolded)) {
									if (cardsHolded[jj]->value == SecondFigures[ii]) {
										for (k = 0; k < cardGroup.type.cnt2; k++)
											//cout<<"添加"<<cardsHolded[j]->value+3<<endl;
											ret.push_back(cardsHolded[jj + k]);
										do {
											jj++;
										} while (jj < SZ(cardsHolded) && cardsHolded[jj]->value == SecondFigures[ii]);
									}
									else jj++;
								}
							}
							return ret;
						}//if(findSecondFigure) 
					}//end if(cardGroup.type.cnt2>0)
					else
						return ret;
				}//end if(findFirstFigure)
			}//end for(i=value+1;i<VALUECOUNT;i++)
		}
		ret.clear();
		//没牌打得过时查找有没有炸弹 
		if (typeStr != "炸弹")
			for (int i = SZ(cardsHolded) - 1; i >= 3; i--)
				if (cardsHolded[i]->value == cardsHolded[i - 3]->value) {
					rep(j, 0, 4)
						ret.push_back(cardsHolded[i - j]);
					break;
				}
		return ret;
	}//end vector<Card*> findCanBeatFrom()
};
LastCards* LastCards::lastCards = NULL;
class Player {
public:
	string name;
	vector<Card*> cards;
	void arrange() { stable_sort(cards.begin(), cards.end(), cmp); }
	void print() {
		O << this->name << ":\t";
		for (int i = 0; i < SZ(cards); i++) cards[i]->print();
		O << "[" << SZ(cards) << "]\n";
	}
	vector<Card*> tip() {
		//提示功能,使自己最小一张连最长
		CardGroup ret;
		string temp;
		int j, k, m = SZ(cards);
		for (j = 0; j < m; j++) {
			temp = "";
			for (k = j; k < m; k++) temp += cards[k]->figure;
			ret.init(temp, cards);
			if (ret.type.typeId != ERROR) return ret.cards;
		}
		ret.cards.clear();
		return ret.cards;
	}
	void chupai(CardGroup& cardGroup) {
		//出牌 
		O << this->name << ":\t";
		O << cardGroup.type.typeStr << ' ';
		for (int i = 0; i < SZ(cardGroup.cards); i++) {
			cardGroup.cards[i]->print();
			this->cards.erase(find(this->cards.begin(), this->cards.end(), cardGroup.cards[i]));
		}
		O << "\t[" << SZ(this->cards) << "]\n";
		if (cardGroup.type.typeStr != "不出") {
			LastCards::inst()->player = this;
			LastCards::inst()->cardGroup.init(cardGroup.cards);
		}
	}
};
class Landlords {
	Player* player[PLAYERCOUNT];
	bool finished = false, youWin = false, landlordWin = false;
	int landlordIndex = 0;
	Card* cards[CARDSCOUNT];
public:
	Landlords() {
		int i = 0, j = 0, k = 0;
		for (i = 0; i < PLAYERCOUNT; i++) this->player[i] = new Player();
		//54张牌初始化 
		for (k = i = 0; i < 14; i++) {
			if (toFigure[i] == ' ') continue;
			for (decltype(eHEART) color = eHEART; color <= eSPADE; color = decltype(eHEART)(color + 1)) this->cards[k++] = new Card(toFigure[i], color);
		}
		this->cards[k++] = new Card('Y', eSPADE);
		this->cards[k] = new Card('Z', eHEART);
	}
	~Landlords() {
		rep(i, 0, PLAYERCOUNT) free(this->player[i]);
		rep(i, 0, CARDSCOUNT) free(this->cards[i]);
	}
	void init() {
		player[CURRENTPLAYER]->name = "Bice";
		player[1]->name = "玩家2";
		player[2]->name = "玩家3";
		finished = youWin = landlordWin = false;
		landlordIndex = ERROR;
		while (landlordIndex == ERROR) {
			srand((int)time(0)); shuffle();
			landlordIndex = chooseLandlord();
		}
		O << player[landlordIndex]->name << "\t成为地主\n\n";
		this->add3Cards();
		LastCards::inst()->player = player[landlordIndex];
	}
	void startGame() {
		string inputSrt;
		CardGroup inputCards;
		for (int iTurns = landlordIndex; !finished; iTurns++) {
			if (iTurns >= PLAYERCOUNT) iTurns = 0;
			if (iTurns == CURRENTPLAYER) {
				O << '\n';
				player[iTurns]->print();
				O << "输入提示：Z=大王 Y=小王 0=10 N=不要 输入可无序 例如:JKQ0A9\n请出牌：\t";
				do {
					I >> inputSrt;
					inputCards.init(inputSrt, player[iTurns]->cards);
				} while (!check(&inputCards));
			}
			else {
				//若是上次出牌的是自己，启用提示功能 
				if (player[iTurns] == LastCards::inst()->player) inputCards.init(player[iTurns]->tip());
				//否则查找能打得过上家的牌 
				else inputCards.init(LastCards::inst()->findCanBeatFrom(player[iTurns]->cards));
			}
			player[iTurns]->chupai(inputCards);//出牌 
			if (SZ(player[iTurns]->cards) == 0) {
				//玩家手中没牌了，游戏结束 
				finished = true;
				landlordWin = iTurns == landlordIndex;
				if (landlordWin) youWin = landlordIndex == CURRENTPLAYER;
				else youWin = landlordIndex != CURRENTPLAYER;
			}
		}
		O << "\n_________________________ " << (youWin ? "You Win!" : "You Lose!") << " _________________________\n\n";
	}
	void add3Cards() {
		O << "地主3张牌:\t";
		for (int i = PLAYERCOUNT * 17; i < CARDSCOUNT; i++) {
			this->cards[i]->print();
			player[landlordIndex]->cards.push_back(cards[i]);
		}
		O << endl;
		player[landlordIndex]->arrange();
	}
	int chooseLandlord() {
		O << "\n_________________________ 抢地主 _________________________\n\n";
		int first = -1, last, cnt = 0, i, j = rand() % PLAYERCOUNT;
		bool decision;
		for (i = 0; i < PLAYERCOUNT; i++, j == 2 ? j = 0 : j++) {
			if (j == CURRENTPLAYER) decision = makeChoice("是否抢地主？(Y=抢/N=不抢):");
			else decision = rand() % 2;
			if (decision) {
				cnt++;
				last = j;
				if (first == -1)
					first = j;
				O << this->player[j]->name << "\t抢地主\n";
			}
			else
				O << this->player[j]->name << "\t没有抢\n";
		}
		if (cnt == 0) {
			O << "没人抢，重新发牌\n";
			return ERROR;
		}
		if (cnt == 1) {
			//第一轮只有一人抢地主 
			return first;
		}
		else {
			//最后一次争抢 
			if (first == CURRENTPLAYER) decision = makeChoice("是否抢地主？(Y=抢/N=不抢):");
			else decision = rand() % 2;
			if (decision) {
				O << this->player[first]->name << "\t抢地主\n";
				return first;
			}
			else {
				O << this->player[first]->name << "\t没有抢\n";
				return last;
			}
		}
	}
	void shuffle() {
		//洗牌 
		rep(i, 0, CARDSCOUNT) swap(this->cards[i], this->cards[rand() % CARDSCOUNT]);
		//分牌 
		int k = 0;
		rep(i, 0, PLAYERCOUNT) {
			this->player[i]->cards.clear();
			rep(j, 0, 17) this->player[i]->cards.push_back(this->cards[k++]);
			this->player[i]->arrange(); this->player[i]->print();
		}
	}
	bool check(CardGroup* cardGroup) {
		if (cardGroup->type.typeId == ERROR) {
			O << "出牌错误，重新输入\n";
			return false;
		}
		else if (cardGroup->type.typeStr == "不出") return true;
		else if (LastCards::inst()->player != player[CURRENTPLAYER] && !cardGroup->isCanBeat(LastCards::inst()->cardGroup)) {
			O << "打不过，重新输入\n";
			return false;
		}
		else return true;
	}
};
void SetPos(COORD a) { HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE); SetConsoleCursorPosition(out, a); }
void SetPos(int i, int j) { COORD pos = { (SHORT)i, (SHORT)j }; SetPos(pos); }
void color(int a) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), a); }
void Pos() {
	decltype(GetStdHandle(STD_OUTPUT_HANDLE)) hOut = GetStdHandle(STD_OUTPUT_HANDLE); // 每个计算机的handle不一样
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hOut, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cci);
}
void logo() { return; }
void logoing() { return; }
void lobby() {
	system("TITLE 小游戏"); system("cls");
	Pos();
	cout << '\n';
	logo();
	SetPos(10, 5);
	color(12);
	cout << "欢迎使用多功能程序";
	SetPos(0, 20);
	O << ("----------------------------------------------------------\n");
	O << ("     版权所有 FJ a97 OwenLMZ    改版 SFWY3 v1.0.9");
	color(12);
	SetPos(1, 7);
	O << "┌───────────────┐\n │               │\n │  计算器[a]    │\n │               │\n └───────────────┘\n";
	color(10);
	SetPos(23, 7);
	O << "┌───────────────┐\n";
	SetPos(23, 8);
	O << "│               │\n";
	SetPos(23, 9);
	O << "│ 各种小游戏[s] │\n";
	SetPos(23, 10);
	O << "│               │\n";
	SetPos(23, 11);
	O << "└───────────────┘\n";
	color(11);
	SetPos(1, 13);
	O << "┌───────────────┐\n │               │\n │  去抄题解[d]  │\n │               │\n └───────────────┘\n";
	color(14);
	SetPos(23, 13);
	O << "┌───────────────┐\n";
	SetPos(23, 14);
	O << "│               │\n";
	SetPos(23, 15);
	O << "│  系统操作[f]  │\n";
	SetPos(23, 16);
	O << "│               │\n";
	SetPos(23, 17);
	O << "└───────────────┘\n";
	Pos();
}
void calculator1() {
	system("cls");
	printf("\n\n\n\n\n");
	logoing();
	system("cls");
	color(11);
	printf("|计算器|输入1!1退出\n");
	color(14);
	while (1) {
		double num1, num2;
		char op;
		double result;
		string caption1("除数不能为0!结果为∞\n");
		string caption2("Invalid operator!\n");
		color(7);
		I >> num1 >> op >> num2;
		if (!cin) {
			color(11);
			O << "\n你是来砸场子的吧！\n";
			color(7);
			Sleep(500);
			system("pause");
			break;
		}
		color(15);
		switch (op) {
		case '+':
			result = num1 + num2;
			O << num1 << op << num2 << "=" << result << '\n'; break;
		case '-':
			result = num1 - num2;
			O << num1 << op << num2 << "=" << result << '\n'; break;
		case '*':
			result = num1 * num2;
			O << num1 << op << num2 << "=" << result << '\n'; break;
		case '/':
			if (fabs(num2) < 1.0e-8) O << caption1 << '\n';
			else {
				result = num1 / num2;
				O << num1 << op << num2 << "=" << result << '\n';  break;
			}
		case '!': goto loop_calculator1;
		default: { O << caption2; }
		}
	}
loop_calculator1:
	return;
}
void calculator2_1() {
	system("cls");
	color(14);
	SetPos(1, 1);
	printf("|高精度加法|输入   退出\n");
	color(11);
	printf(" 每次输入两行，一行一个数，自动相加\n\n");
	color(10);
	SetPos(17, 1);
	printf("[E]");
	SetPos(0, 5);
	while (1) {
		int a1_len, b1_len, lenc, i, x;
		memset(a, 0, sizeof(a));
		memset(b, 0, sizeof(b));
		memset(c, 0, sizeof(c));
		color(11);
		cin >> a1;
		if (a1[0] == 'E' || a1[0] == 'e') break;
		cin >> b1; // gets_s(b1) 可能无结尾
		a1_len = (int)strlen(a1);
		b1_len = (int)strlen(b1);
		for (i = 0; i <= a1_len - 1; i++) a[a1_len - i] = a1[i] - 48; //将操作数放入a数组 　
		for (i = 0; i <= b1_len - 1; i++) b[b1_len - i] = b1[i] - 48; //将操作数放入b数组 
		lenc = 1;
		x = 0;
		while (lenc <= a1_len || lenc <= b1_len) {
			c[lenc] = a[lenc] + b[lenc] + x; //两数相加 　
			x = c[lenc] / 10;
			c[lenc] = c[lenc] % 10;
			lenc++;
		}
		c[lenc] = x;
		if (c[lenc] == 0) lenc--;
		for (i = lenc; i >= 1; i--) O << c[i];
		O << '\n' << '\n';
	}
}
void calculator2_2() {
	system("cls");
	color(14);
	SetPos(1, 1);
	printf("|高精度减法|输入   退出\n");
	color(11);
	printf("每次输入两行，一行一个数，自动相减\n\n");
	color(10);
	SetPos(17, 1);
	printf("[E]");
	SetPos(0, 5);
	while (1) {
		color(7);
		const int N = 10001;
		memset(a, 0, sizeof(a));
		memset(b, 0, sizeof(b));
		memset(c, 0, sizeof(c));
		//gets_s(n1);
		cin >> n1;
		if (n1[0] == 'E') break;
		//gets_s(n2);
		cin >> n2;
		color(15);
		size_t lena = strlen(n1), lenb = strlen(n2);
		if (lena < lenb || (lena == lenb && strcmp(n1, n2) < 0)) {
			strcpy_s(n, n1);
			strcpy_s(n1, n2);
			strcpy_s(n2, n);
			swap(lena, lenb);
			cout << '-';
		}
		for (i = 0; i < lena; i++) a[lena - i] = int(n1[i] - '0');
		for (i = 0; i < lenb; i++) b[lenb - i] = int(n2[i] - '0');
		i = 1;
		while (i <= lena || i <= lenb) {
			if (a[i] < b[i]) {
				a[i] += 10;
				a[i + 1] --;
			}
			c[i] = a[i] - b[i];
			i++;
		}
		int lenc = i;
		while (c[lenc] == 0 && lenc > 1)
			lenc--;
		for (i = lenc; i >= 1; i--)
			printf("%d", c[i]);
		printf("\n\n");
	}
}
void calculator2_3() {
	system("cls");
	color(14);
	SetPos(1, 1);
	printf("|高精度乘法|输入   退出\n");
	color(11);
	printf("每次输入两行，一行一个数，自动相乘\n\n");
	color(10);
	SetPos(17, 1);
	printf("[E]");
	SetPos(0, 5);
	while (1) {
		string str1, str2;
		int i, j, len;
		memset(a, 0, sizeof(a));
		memset(b, 0, sizeof(b));
		color(8);
		I >> str1;
		if (str1[0] == 'E')
			break;
		I >> str2;
		color(15);
		a[0] = (int)str1.length();
		for (i = 1; i <= a[0]; i++)
			a[i] = str1[a[0] - i] - '0';
		b[0] = (int)str2.length();
		for (i = 1; i <= b[0]; i++)
			b[i] = str2[b[0] - i] - '0';
		memset(c, 0, sizeof(c));
		for (i = 1; i <= a[0]; i++)
			for (j = 1; j <= b[0]; j++) {
				c[i + j - 1] += a[i] * b[j];
				c[i + j] += c[i + j - 1] / 10;
				c[i + j - 1] %= 10;
			}
		len = a[0] + b[0];
		while ((c[len] == 0) && (len > 1)) len--;
		for (i = len; i >= 1; i--)
			O << c[i];
		O << ("\n\n");
	}
}
void calculator2() {
	system("cls");
	O << "\n\n\n\n\n";
	logoing();
loop_calculator2_head:
	system("cls");
	color(10);
	SetPos(1, 1);
	O << ("|高精度计算器| 按   返回");
	color(11);
	SetPos(18, 1);
	O << ("[`]");
	color(14);
	O << ("\n\n ————————————");
	SetPos(1, 5);
	O << ("|高精度加法| [s]");
	SetPos(1, 7);
	O << ("|高精度减法| [d]");
	SetPos(1, 9);
	O << ("|高精度乘法| [f]");
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			switch (c) {
			case 's': {
				calculator2_1();
				goto loop_calculator2_head;
			}
			case 'd': {
				calculator2_2();
				goto loop_calculator2_head;
			}
			case 'f': {
				calculator2_3();
				goto loop_calculator2_head;
			}
			case '`':
				goto loop_calculator2_tail;
			}
		}
loop_calculator2_tail:
	return;
}
void calculator3(char op) { // 魏启轩自制
	O << "\n\n\n\n\n";
	logoing();
	system("cls");
	int a, b, c, d;
	color(10);
	SetPos(1, 1);
	O << ("|分数计算器| 按         返回");
	SetPos(16, 1);
	O << ("[0,0,0,0]");
	color(14);
	O << ("\n\n ————————————\n");
	while (1) {
		color(15);
		O << "请输入第一个分数的分子:"; I >> a;
		if (!cin) return;
		O << "请输入第一个分数的分母:"; I >> b;
		if (!cin) return;
		O << "请输入第二个分数的分子:"; I >> c;
		if (!cin) return;
		O << "请输入第二个分数的分母:"; I >> d;
		if (!cin) return;
		if (!a && !b && !c && !d) break;
		frac a1, b1;
		a1.n = a; a1.de = b; b1.n = c; b1.de = d;
		// frac 部分 没有+=/-=/*=/ /=
		if (op == '+')a1 + b1;
		else if (op == '-')a1 - b1;
		else if (op == '*')a1* b1;
		else if (op == '/')a1 / b1;
		else return;
		color(11);
		O << "结果: (取绝对值后)分子: " << a1.n << "分母: " << a1.de << '\n';
	}
}
void calculator3d() {
	system("cls");
	O << "\n\n\n\n\n";
	logoing();
loop_calculator4_head:
	system("cls");
	color(10);
	SetPos(1, 1);
	O << ("|分数计算器| 按   返回");
	color(11);
	SetPos(16, 1);
	O << ("[`]");
	color(14);
	O << ("\n\n ————————————");
	SetPos(1, 5);
	O << ("|分数加法| [a]");
	SetPos(1, 7);
	O << ("|分数减法| [s]");
	SetPos(1, 9);
	O << ("|分数乘法| [d]");
	SetPos(1, 11);
	O << ("|分数除法| [f]");
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			switch (c) {
			case 'a': {
				calculator3('+');
				goto loop_calculator4_head;
			}
			case 's': {
				calculator3('-');
				goto loop_calculator4_head;
			}
			case 'd': {
				calculator3('*');
				goto loop_calculator4_head;
			}
			case 'f': {
				calculator3('/');
				goto loop_calculator4_head;
			}
			case '`':
				goto loop_calculator4_tail;
			}
		}
loop_calculator4_tail:
	return;
}
const int maxN = 1007; // 我猜长宽超过这个常数的矩阵普通人也打不出来
long mat1[maxN][maxN]{}, mat2[maxN][maxN]{}, tmp[maxN][maxN];
void matrix_plus(char ch) {
	system("cls");
	O << "加载中...";
	Sleep(500);
	system("cls");
	color(10);
	SetPos(1, 1);
	if (ch == '+')O << ("|矩阵加法| 输入    返回");
	else if (ch == '-')O << ("|矩阵减法| 输入    返回");
	SetPos(16, 1);
	O << ("[-1]");
	O << "\n\n";
	color(14);
	while (1) {
		int  c = 0, d = 0;
		O << "请输入矩阵的长: "; I >> c;
		if (c == -1) goto sb;
		else if (!cin || c <= 0) goto text;
		O << "请输入矩阵的宽: "; I >> d;
		if (c == -1) goto sb;
		else if (!cin || d <= 0) goto text;
		O << "请输入第一个矩阵\n";
		rep(i, 1, c + 1) rep(j, 1, d + 1) { I >> mat1[i][j]; if (!cin) goto text; }
		O << "请输入第二个矩阵\n";
		rep(i, 1, c + 1) rep(j, 1, d + 1) {
			I >> mat2[i][j];
			if (!cin) goto text;
		}
		O << "结果\n";
		rep(i, 1, c + 1) {
			rep(j, 1, d + 1)
				O << (ch == '+' ? mat1[i][j] + mat2[i][j] : mat1[i][j] - mat2[i][j]) << ' ';
			O << '\n';
		}
	}
sb:
	return;
text:
	system("cls");
	cout << "你是来砸场子的吧！！不欢迎你！";
	Sleep(1000);
}
void matrix_mult() {
	system("cls");
	O << "加载中...";
	Sleep(500);
	system("cls");
	color(10);
	SetPos(1, 1);
	O << ("|矩阵乘法| 输入    返回");
	SetPos(16, 1);
	O << ("[-1]");
	O << "\n\n";
	color(14);
	while (1) {
		int c = 0, d = 0, e = 0, f = 0;
		fflush(stdin);
		O << "请输入第一个矩阵的长: "; I >> c;
		if (c == -1 || c <= 0) goto sb;
		else if (!cin) goto text;
		O << "请输入第一个矩阵的宽: "; I >> d;
		if (d == -1) goto sb;
		else if (!cin || d <= 0) goto text;
		O << "请输入第一个矩阵\n";
		rep(i, 1, c + 1) rep(j, 1, d + 1) { I >> mat1[i][j]; if (!cin) goto text; }
		O << "请输入第二个矩阵的长: "; I >> e;
		if (e == -1) goto sb;
		else if (!cin || e <= 0) goto text;
		O << "请输入第二个矩阵的宽: "; I >> f;
		if (f == -1) goto sb;
		else if (!cin || f <= 0) goto text;
		O << "请输入第二个矩阵\n";
		rep(i, 1, e + 1) rep(j, 1, f + 1) { I >> mat2[i][j]; if (!cin) goto text; }
		rep(i, 1, c + 1)
			rep(j, 1, f + 1) {
			tmp[i][j] = 0;
			rep(k, 1, d + 1) tmp[i][j] += mat1[i][k] * mat2[k][j];
		}
		O << "结果\n";
		rep(i, 1, c + 1) {
			rep(j, 1, f + 1)
				cout << tmp[i][j] << ' ';
			cout << '\n';
		}
	}
sb:
	return;
text:
	system("cls");
	cout << "你是来砸场子的吧！！不欢迎你！";
	Sleep(1000);
}
void calculator_matrix() {
	system("cls"); O << "\n\n\n\n\n"; logoing();
loop_calculator5_head:
	system("cls"); color(10); SetPos(1, 1);
	O << ("|矩阵计算器(Beta)| 按   返回"); color(11);
	SetPos(22, 1); O << ("[`]");
	color(14); O << ("\n\n ————————————");
	SetPos(1, 5); O << ("|矩阵加法| [a]");
	SetPos(1, 7); O << ("|矩阵减法| [s]");
	SetPos(1, 9); O << ("|矩阵乘法| [d]");
	char c;
	{
		while (1)
			if (_kbhit()) {
				c = _getch();
				switch (c) {
				case 'a': {
					matrix_plus('+');
					goto loop_calculator5_head;
				}
				case 's': {
					matrix_plus('-');
					goto loop_calculator5_head;
				}
				case 'd': {
					matrix_mult();
					goto loop_calculator5_head;
				}
						// 矩阵无法作除
				case '`':
					goto loop_calculator5_tail;
				}
			}
	}
loop_calculator5_tail:
	return;
}
void game1() {
	system("cls");
	color(6);
	char maze[18][24] = {
	"************** ********",//1
	"*     #      * *      *",//2
	"* ************ * ** * *",//3
	"*            * **** * *",//4
	"* ********** * *    * *",//5
	"* **       *      *****",//6
	"* ** ***** ***** * ** *",//7
	"*        *          * *",//8
	"****** * **********   *",//9
	"*    * * *        * $ *",//10
	"* **** * * ****** *****",//11
	"* *    * * *    * *   *",//12
	"* * ****** * ** * * * *",//13
	"* *        * ** * * * *",//14
	"* ********** **     * *",//15
	"*               *     *",//16				  				  				            		
	"*************** *******",//17 
	"***********************" };//18				
	int x, y;
	int count = 0;
	srand((uint32_t)time(0));
	x = rand() % 18;
	y = rand() % 23;
	while (maze[x][y] != ' ')
		x = rand() % 18,
		y = rand() % 23;
	maze[x][y] = 'i';
	rep(i, 0, 18) {
		rep(j, 0, 23)
			O << maze[i][j] << " ";
		O << '\n';
	}
	char c;
	while (true) {
		c = _getch();
		++count;
		system("cls");
		if (c == 'w') {
			if (maze[x - 1][y] == ' ' || maze[x - 1][y] == '$') {
				maze[x][y] = ' ';
				--x;
				maze[x][y] = 'i';
			}
			else {
				MessageBox(0, strtolp("你撞到了墙上/炸弹上，输掉了游戏"), strtolp("   警告"), MB_OK);
				break;
			}
		}
		if (c == 'a') {
			if (maze[x][y - 1] == ' ' || maze[x][y - 1] == '$') {
				maze[x][y] = ' ';
				--y;
				maze[x][y] = 'i';
			}
			else {
				MessageBox(0, strtolp("你撞到了墙上/炸弹上，输掉了游戏"), strtolp("   警告"), MB_OK);
				break;
			}
		}
		if (c == 's') {
			if (maze[x + 1][y] == ' ' || maze[x + 1][y] == '$') {
				maze[x][y] = ' ';
				++x;
				maze[x][y] = 'i';
			}
			else {
				MessageBox(0, strtolp("你撞到了墙上/炸弹上，输掉了游戏"), strtolp("   警告"), MB_OK);
				break;
			}
		}
		if (c == 'd') {
			if (maze[x][y + 1] == ' ' || maze[x][y + 1] == '$') {
				maze[x][y] = ' ';
				++y;
				maze[x][y] = 'i';
			}
			else {
				MessageBox(0, strtolp("你撞到了墙上/炸弹上，输掉了游戏"), strtolp("   警告"), MB_OK);
				break;
			}
		}
		rep(i, 0, 18) {
			rep(j, 0, 23)
				O << maze[i][j] << " ";
			O << '\n';
		}
		if (maze[9][20] == 'i') {
			maze[0][14] = ' ';
			MessageBox(0, strtolp("取到宝箱"), strtolp("提示"), MB_OKCANCEL);
		}
		if (x == 0 && y == 14 && maze[9][20] == ' ') {
			ss steps;		//messagebox显示使用步数 				
			steps << "走出迷宫，使用步数为:" << count;
			MessageBox(0, strtolp(steps.str().c_str()), strtolp("提示"), MB_OKCANCEL);//将获得的字符串显示在messagebox上 
			break;
		}
		if (x < 0) {
			MessageBox(0, strtolp("请重新开始"), strtolp("   警告"), MB_OK);
			break;
		}
	}
	Sleep(2000);
	return;
}
void game2() {
	color(6);
	int num[5][5] = { 0 },//16张卡牌 
		arr[5][5] = { 0 },//标记卡牌是否被翻出 
		ix, iy, ia, ib, cnt = 0,
		flag[9] = { 0 },//1-8在num数组中出现的次数 
		temp;
	O << "记忆卡牌规则:\n";
	O << "现有16张卡牌，由数字1-8组成，每个数字出现2次\n";
	O << "游戏者需要将卡牌翻至背面，找出所有相同的数字\n";
	O << "每一轮只允许翻两张卡牌" << endl;
	O << "如果卡牌数字相同，则将卡牌上有数字的一面显示出来\n";
	O << "否则将卡牌翻回正面，进行下一轮\n";
	O << "系统测量游戏者找出所有相同数字的卡牌所花费的步数(进行多少轮)\n";
	system("pause");
	srand((uint32_t)time(0));
	rep(i, 1, 5) {
		rep(j, 1, 5) {
			while (true) {
				temp = rand() % 8 + 1;
				if (flag[temp] < 2) {
					num[i][j] = temp;
					flag[temp]++;
					break;
				}
			}
		}
	}
	rep(i, 1, 5) {
		rep(j, 1, 5) O << '*';
		cout << '\n';
	}
	while (true) {
		cnt++;
		int sum = 0;
		O << "请输入第一次翻开卡片的行数和列数:\n";
		I >> ix >> iy;
		arr[ix][iy]++;
		system("cls");
		rep(i, 1, 5) {
			rep(j, 1, 5) {
				if (arr[i][j] > 0) O << num[i][j];
				else  O << "*";
			}
			O << '\n';
		}
		O << "请输入第二次翻开卡片的行数和列数:\n";
		I >> ia >> ib;
		arr[ia][ib]++;
		system("cls");
		rep(i, 1, 5) {
			rep(j, 1, 5) {
				if (arr[i][j] > 0) O << num[i][j];
				else  O << "*";
			}
			O << '\n';
		}
		if (num[ix][iy] == num[ia][ib])
			O << "恭喜您，此轮翻牌成功\n";
		else {
			O << "很遗憾，此轮翻牌失败\n";
			arr[ix][iy]--;
			arr[ia][ib]--;
		}
		O << "2秒后进入下一轮\n";
		Sleep(2000);
		system("cls");
		rep(i, 1, 5) {
			rep(j, 1, 5) {
				if (arr[i][j] > 0) {
					sum += 1;
					O << num[i][j];
				}
				else O << "*";
			}
			O << '\n';
		}
		if (sum == 16) {
			O << '\n';
			O << "一共使用" << cnt << "步\n";
			break;
		}
	}
	Sleep(2000);
}
void game3() {
	outArr();
	int x = 1, y = 1, count = 0;
	while (true) {
		O << "请输入移动方向(wasd): \n";
		char choose;
		choose = _getch();
		++count;
		if (choose == 'w') {
			if (x >= 0 && x < 2)
				change(&n_puzzle[x][y], &n_puzzle[x + 1][y]);
			++x;
		}
		if (choose == 'a')
			if (y >= 0 && y < 2) {
				change(&n_puzzle[x][y], &n_puzzle[x][y + 1]);
				++y;
			}
		if (choose == 's')
			if (x > 0 && x <= 2) {
				change(&n_puzzle[x][y], &n_puzzle[x - 1][y]);
				--x;
			}
		if (choose == 'd')
			if (y > 0 && y <= 2) {
				change(&n_puzzle[x][y], &n_puzzle[x][y - 1]);
				--y;
			}
		system("cls");
		outArr();
		int flag = 1;
		rep(i, 0, 3)
			rep(j, 0, 3)
			if (n_puzzle[i][j] != n_puzzle_3[i][j])
				flag = 0;
		if (flag == 1) {
			ss s;
			s << "完成数字华容道，使用步数为:" << count;
			MessageBox(0, strtolp(s.str().c_str()), strtolp("提示"), MB_OK);//将获得的字符串显示在messagebox上 
			//MessageBox(0,"恭喜你完成数字华容道","提示",MB_OK);
			break;
		}
	}
}
void game4() {
	O << "这是一个武侠的世界\n";
	Sleep(500);
	O << "为了参加天下第一武道大会\n";
	Sleep(500);
	O << "玩家需要操作小码君进行为期一年的特训\n";
	Sleep(500);
	O << "只有总属性超过500才能参加大会\n";
	Sleep(500);
	O << "在一年中,每个月小码君都可以进行一次探险\n\n";
	Sleep(500);
	O << "挖矿：增加攻击力和金钱，但是会减少少量内功\n";
	Sleep(500);
	O << "闭关：随机增加或减少攻击和内功，但是会消耗金钱\n";
	Sleep(500);
	O << "吃饭：增加大量生命，但是会消耗很多金钱\n";
	Sleep(500);
	O << "闯荡：出门闯荡，触发战斗\n";
	Sleep(500);
	O << "闲逛：购买装备增加自身属性，但是会消耗金钱\n\n";
	Sleep(500);
	O << "准备好开始冒险了吗？\n";
	system("pause");
	// name of item
	char item[10][9] = {
	"铁剑", "蓝水晶", "布甲",
	"大剑", "红玛瑙", "贤者之石",
	"风暴巨剑", "力量腰带", "霸者重装",
	"破军" };
	int prices[10] = { 10, 10, 10, 20, 20, 20, 50, 50, 100, 100 };
	// attributes
	int itemAtt[10][4] =
	{ {10, 0, 0, 0}, {0, 0, 10, 0}, {0, 0, 0, 10},
	{20, 0, 0, 0}, {0, 20, 0, 0}, {0, 10, 10 , 0},
	{50, 0, 0, 0}, {0, 50, 0, 0}, {0, 100, 0, 0},
	{100, 0, 0, 0} };
	int itemList[10]{};
	rep(i, 0, 10) itemList[i] = -1;
	string enemyName[5] = { "毛贼", "强盗", "杀手", "头目", "擂主" };
	int enemyAtt[5][4] =
	{ {60, 60, 50, 0},
	{70, 60, 60, 0},
	{90, 60, 50, 20},
	{90, 70, 80, 30},
	{100, 70, 90, 50} };
	//敌人赏金 
	int enemyMoney[5] = { 30, 30, 50, 50, 60 };
	//显示四个随机生成的属性并选定角色属性-------------------------------------------------------------------------------------	
	char playAtt[6][5] = { "攻击", "血量", "内功", "防御", "金钱" };
	//人物属性 
	int player[6] = {};//0.攻击 1.血量 2.内功 3.防御 4.金钱 
	srand((uint32_t)time(0));
	while (true) {
		system("cls");
		O << "请选择你的初始属性：\n";
		//随机角色的4个属性
		rep(i, 0, 4)
			player[i] = rand() % 50 + 40;
		player[4] = 50;
		//显示角色的4个属性
		O << "当前角色信息\n";
		rep(i, 0, 5) {
			if (player[i] >= 120) color(6);
			else if (player[i] >= 80) color(4);
			O << playAtt[i] << ":" << player[i] << '\n';
			color(7);
		}
		char user;//定义一个user，用来确定角色属性
		O << "是否选择初始属性？（y/n）" << '\n';
		user = getch();
		if (user == 'y') break;
	}
	system("cls");
	O << "正在创建角色\n";
	for (int i = 3; i >= 1; --i) {
		Sleep(1000);
		O << i << '\n';
	}
	O << "角色创建成功！！\n";
	//显示角色信息
	O << "当前角色信息\n";
	rep(i, 0, 5) {
		if (player[i] >= 120) color(6);
		else if (player[i] >= 80) color(4);
		else color(7);
		O << playAtt[i] << " :" << player[i] << '\n';
		color(7);
	}
	system("pause");
	//第三步，12个月的修炼
	//当前月份 
	int month = 1;
	while (month <= 12) {
		system("cls");
		while (true) {
			O << "现在是" << month << "月\n";
			O << "q 查看属性\n";
			O << "a 挖矿 \n";
			O << "s 闭关 \n";
			O << "d 吃饭 \n";
			O << "f 闯荡 \n";
			O << "g 闲逛 \n";
			O << "请输入：";
			char choose;//用来输入选项
			I >> choose;
			if (choose == 'q') {
				system("cls");
				//显示角色属性
				O << "当前角色信息\n";
				rep(i, 0, 5) {
					if (player[i] >= 120) color(6);
					else if (player[i] >= 80) color(4);
					else color(7);
					O << playAtt[i] << " :" << player[i] << '\n';
					color(7);
				}
				//显示装备
				O << "当前装备：\n";
				if (itemList[0] == -1) O << "没有装备\n";
				rep(i, 0, 10) {
					if (itemList[i] == -1) break;
					else if (itemList[i] < 3) color(15);
					else if (itemList[i] < 6) color(10);
					else if (itemList[i] < 8) color(1);
					else color(5);
					O << item[itemList[i]] << '\n';
					color(15);
				}
			}
			else if (choose == 'a') {
				O << "小码君疯狂挖矿中\n";
				O << "嘿咻！！嘿咻！！金钱+5\n";
				Sleep(800);
				O << "嘿咻！！嘿咻！！金钱+5\n";
				Sleep(800);
				O << "小码君加强了麒麟臂！！攻击+10\n";
				Sleep(800);
				O << "小码君荒废了功力，内功-5\n";
				Sleep(800);
				player[4] += 10;
				player[0] += 10;
				player[2] -= 5;
				break;
			}
			else if (choose == 's') {
				O << "小码君疯狂修炼中\n";
				char event[2][100] = { "感悟天道，功力大增，攻击+20，内功+20",
				"走火入魔，功力大减，攻击-15，内功-15" };
				rep(i, 0, 3) {
					int temp = rand() % 2;
					O << event[temp] << '\n';
					Sleep(1000);
					if (temp == 0) {
						player[0] += 20;
						player[2] += 20;
					}
					else if (temp == 1) {
						player[0] -= 15;
						player[2] -= 15;
					}
					else player[4] += 30;
				}
				break;
			}
			else if (choose == 'd') {
				O << "小码君贼能吃\n";
				rep(i, 0, 3) {
					short temp = rand() % 20 + 1;
					O << "小码君疯狂吃东西" << '\n';
					Sleep(2000);
					O << "血量+" << temp << ",金钱-" << temp / 2 + 1 << '\n';
					player[1] += temp;
					player[4] -= temp / 2 + 1;
				}
				break;
			}
			else if (choose == 'f') {
				O << "欢迎进行闯荡，发现敌人：\n";
				short temp = rand() % 5;
				if (enemyAtt[temp][1] <= 0) temp = rand() % 5;
				O << enemyName[temp] << '\n';
				rep(i, 0, 4) cout << playAtt[i] << " : " << enemyAtt[temp][i] << '\n';
				O << "赏金 : " << enemyMoney[temp] << '\n';
				O << "当前角色信息" << endl;
				rep(i, 0, 4) {
					if (player[i] >= 120) color(6);
					else if (player[i] >= 80) color(12);
					else color(15);
					O << playAtt[i] << " ：" << player[i] << '\n';
					color(15);
				}
				O << "是否迎战？(y/n)\n";
				char user;
				I >> user;
				if (user == 'y') {
					if (enemyAtt[temp][2] > player[2])
						O << "内功不够，被打劫了。。金币-20\n",
						player[4] -= 20;
					else {
						while (enemyAtt[temp][1] > 0) {
							if (player[1] <= 0) break;
							int attack1 = enemyAtt[temp][0] - player[3];
							if (attack1 < 0) attack1 = 0;
							O << enemyName[temp] << "发动攻击,造成" << attack1 << "伤害\n";
							player[1] -= attack1;
							int attack2 = player[0] - enemyAtt[temp][3];
							if (attack2 < 0) attack2 = 0;
							O << "小码君发动攻击,造成" << attack2 << "伤害\n";
							enemyAtt[temp][1] -= attack2;
						}
						if (player[1] > 0)
							O << "战斗胜利！！！！！获得赏金" << enemyMoney[temp] << '\n',
							player[4] += enemyMoney[temp];
						//拓展：战斗胜利后，除赏金外，小码君获得其他属性增益 
						//int att = rand()%4;
						//int value = (rand()%3+1) * temp;
						//cout<< playAtt[att]<<"增加"<<value<<endl;
						//player[att]+=value;
					}
				}
				else
					O << "破财免灾，金钱减少10\n",
					player[4] -= 10;
				break;
			}
			else if (choose == 'g') {
				//闲逛
				O << "小码君跋山涉水来到商店\n";
				O << "当前金钱：" << player[4] << '\n';
				short index[5] = { -1,-1,-1,-1,-1 };
				rep(i, 0, 5) {
					index[i] = rand() % 10;
					rep(j, 0, i)
						if (index[i] == index[j]) {
							--i;
							break;
						}
				}
				rep(i, 0, 5) {
					if (index[i] < 3) color(15);
					else if (index[i] < 6) color(10);
					else if (index[i] < 8) color(1);
					else color(5);
					O << (i + 1) << "  " << item[index[i]] << " 价格:" << prices[index[i]] << '\n' << "   " << "装备属性值：";
					for (int j = 0; j < 4; ++j) O << itemAtt[index[i]][j] << "  ";
					O << '\n';
					fflush(stdin);
					color(15);
				}
				O << "请问你要购买几号装备？\n";
				int u;
				I >> u;
				if (u >= 1 && u <= 5 && player[4] >= prices[index[u - 1]]) {
					O << "成功购买" << item[index[u - 1]] << '\n';
					player[4] -= prices[index[u - 1]];
					for (int i = 0; i < 10; ++i) {   //将买完的装备放到装备列表中 
						if (itemList[i] == -1) {
							itemList[i] = index[u - 1];
							break;
						}
					}
					rep(i, 0, 4) {                                                                                 //根据装备属性，提升角色对应属性 
						if (itemAtt[index[u - 1]][i] != 0)
							O << "获得" << playAtt[i] << itemAtt[index[u - 1]][i] << "点\n";
						player[i] += itemAtt[index[u - 1]][i];
					}
				}
				else
					O << "你是来砸场子的吧！！!不欢迎你!\n";
				break;
			}
			else {
				system("cls");
				O << "暂未开通此功能，请重新输入\n";
			}
		}
		system("pause");
		++month;
		rep(i, 0, 5) {
			if (player[i] < 0) O << playAtt[i] << "不足，游戏结束\n";
			if (player[i] <= 10) {
				MessageBox(0, strtolp("请及时查看属性值！"), strtolp("警告"), MB_ICONEXCLAMATION);
				break;
			}
		}
	}
	O << "当前角色信息\n";
	rep(i, 0, 5) {
		if (player[i] >= 120) color(6);
		else if (player[i] >= 80) color(12);
		else color(15);
		O << playAtt[i] << " ：" << player[i] << '\n';
		color(15);
	}
	int sum = 0;
	rep(i, 0, 5) sum += player[i];
	O << "总属性值为：" << sum << '\n';
	system("pause");
	O << "一年后......\n";
	if (sum >= 500) {
		if (player[0] >= 100)
			MessageBox(0, strtolp("恭喜获得攻击达人称号"), strtolp("游戏胜利"), MB_OK);
		if (player[1] >= 100)
			MessageBox(0, strtolp("恭喜获得血量达人称号"), strtolp("游戏胜利"), MB_OK);
		if (player[2] >= 100)
			MessageBox(0, strtolp("恭喜获得内功达人称号"), strtolp("游戏胜利"), MB_OK);
		if (player[3] >= 100)
			MessageBox(0, strtolp("恭喜获得防御达人称号"), strtolp("游戏胜利"), MB_OK);
		if (player[4] >= 100)
			MessageBox(0, strtolp("恭喜获得金钱达人称号"), strtolp("游戏胜利"), MB_OK);
	}
	if (sum < 500) MessageBox(0, strtolp("请来年再战！"), strtolp("游戏失败"), MB_OK);
	Sleep(2000);
}
void game5() {
	system("cls");
	Landlords* landlords = new Landlords();
	do {
		landlords->init();//发牌、抢地主 
		landlords->startGame();//游戏开始 
	} while (makeChoice("\n是否继续游戏？（Y=继续/N=结束）: "));
	(void)landlords;
}
bool makeChoice(string tip) {
	O << tip;
	string input;
	I >> input;
	return input == "Y" || input == "y";
}
bool cmp(Card* a, Card* b) {
	if (a->value == b->value)
		return a->color > b->color;
	else
		return a->value > b->value;
}
void sl() {
	system("cls");
	char a[102][102]{}, b[102][102]{}, c[102][102]{}, w;
	int i, j;  /*循环变量*/
	int x, y, z[999];  /*雷的位置*/
	int t, s;  /*标记*/
	int m, n, lei;  /*计数*/
	int u, v;  /*输入*/
	int hang, lie, ge, mo;  /*自定义变量*/
	i = j = x = y = t = s = m = n = lei = u = v = hang = lie = ge = mo = 0; // init
	srand((int)time(NULL));  /*启动随机数发生器*/
leb1:  /*选择模式*/
	printf("\n   请选择模式：\n   1.标准  2.自定义\n");
	cin >> mo;
	if (mo == 2)  /*若选择自定义模式，要输入三个参数*/
	{
		do {
			t = 0; printf("请输入\n行数 列数 雷的个数\n");
			cin >> hang >> lie >> ge;
			if (hang < 2) { printf("行数太少\n"); t = 1; }
			if (hang > 100) { printf("行数太多\n"); t = 1; }
			if (lie < 2) { printf("列数太少\n"); t = 1; }
			if (lie > 100) { printf("列数太多\n"); t = 1; }
			if (ge < 1) { printf("至少要有一个雷\n"); t = 1; }
			if (ge >= (hang * lie)) { printf("雷太多了\n"); t = 1; }
		} while (t == 1);
	}
	else { hang = 10, lie = 10, ge = 10; }  /*否则就是选择了标准模式(默认参数)*/
	for (i = 1; i <= ge; i = i + 1)  /*确定雷的位置*/
		do {
			t = 0; z[i] = rand() % (hang * lie);
			for (j = 1; j < i; j = j + 1) { if (z[i] == z[j]) t = 1; }
		} while (t == 1);
		for (i = 0; i <= hang + 1; i = i + 1)  /*初始化a,b,c*/
			for (j = 0; j <= lie + 1; j = j + 1) { a[i][j] = '1'; b[i][j] = '1'; c[i][j] = '0'; }
		for (i = 1; i <= hang; i = i + 1)
			for (j = 1; j <= lie; j = j + 1) { a[i][j] = '+'; }
		for (i = 1; i <= ge; i = i + 1)  /*把雷放入c*/
			x = z[i] / lie + 1; y = z[i] % lie + 1; c[x][y] = '#';
		for (i = 1; i <= hang; i = i + 1)  /*计算b中数字*/
			for (j = 1; j <= lie; j = j + 1)
			{
				m = 48;
				if (c[i - 1][j - 1] == '#')m = m + 1; if (c[i][j - 1] == '#')m = m + 1;
				if (c[i - 1][j] == '#')m = m + 1;  if (c[i + 1][j + 1] == '#')m = m + 1;
				if (c[i][j + 1] == '#')m = m + 1;  if (c[i + 1][j] == '#')m = m + 1;
				if (c[i + 1][j - 1] == '#')m = m + 1; if (c[i - 1][j + 1] == '#')m = m + 1;
				b[i][j] = m;
			}
		for (i = 1; i <= ge; i = i + 1)  /*把雷放入b中*/
			x = z[i] / lie + 1; y = z[i] % lie + 1; b[x][y] = '#';
		lei = ge;  /*以下是游戏设计*/
		do
		{
		leb2:  /*输出*/
			system("cls"); printf("\n\n\n\n");

			printf("    ");
			for (i = 1; i <= lie; i = i + 1)
			{
				w = (i - 1) / 10 + 48; printf("%c", w);
				w = (i - 1) % 10 + 48; printf("%c  ", w);
			}
			printf("\n   |");
			for (i = 1; i <= lie; i = i + 1) { printf("---|"); }
			printf("\n");
			for (i = 1; i <= hang; i = i + 1)
			{
				w = (i - 1) / 10 + 48; printf("%c", w);
				w = (i - 1) % 10 + 48; printf("%c |", w);
				for (j = 1; j <= lie; j = j + 1)
				{
					if (a[i][j] == '0')printf("   |");
					else printf(" %c |", a[i][j]);
				}
				if (i == 2)printf(" 剩余雷个数");
				if (i == 3)printf(" %d", lei);
				printf("\n   |");
				for (j = 1; j <= lie; j = j + 1) { printf("---|"); }
				printf("\n");
			}
			cin >> u >> w >> v;
			u = u + 1, v = v + 1;
			if (w != '#' && a[u][v] == '@')
				goto leb2;
			if (w == '#')
			{
				if (a[u][v] == '+') { a[u][v] = '@'; lei = lei - 1; }
				else if (a[u][v] == '@') { a[u][v] = '?'; lei = lei + 1; }
				else if (a[u][v] == '?') { a[u][v] = '+'; }
				goto leb2;
			}
			a[u][v] = b[u][v];
		leb3:  /*打开0区*/
			t = 0;
			if (a[u][v] == '0')
			{
				for (i = 1; i <= hang; i = i + 1)
				{
					for (j = 1; j <= lie; j = j + 1)
					{
						s = 0;
						if (a[i - 1][j - 1] == '0')s = 1; if (a[i - 1][j + 1] == '0')s = 1;
						if (a[i - 1][j] == '0')s = 1;  if (a[i + 1][j - 1] == '0')s = 1;
						if (a[i + 1][j + 1] == '0')s = 1; if (a[i + 1][j] == '0')s = 1;
						if (a[i][j - 1] == '0')s = 1;  if (a[i][j + 1] == '0')s = 1;
						if (s == 1)a[i][j] = b[i][j];
					}
				}
				for (i = 1; i <= hang; i = i + 1)
				{
					for (j = lie; j >= 1; j = j - 1)
					{
						s = 0;
						if (a[i - 1][j - 1] == '0')s = 1; if (a[i - 1][j + 1] == '0')s = 1;
						if (a[i - 1][j] == '0')s = 1;  if (a[i + 1][j - 1] == '0')s = 1;
						if (a[i + 1][j + 1] == '0')s = 1; if (a[i + 1][j] == '0')s = 1;
						if (a[i][j - 1] == '0')s = 1;   if (a[i][j + 1] == '0')s = 1;
						if (s == 1)a[i][j] = b[i][j];
					}
				}
				for (i = hang; i >= 1; i = i - 1)
				{
					for (j = 1; j <= lie; j = j + 1)
					{
						s = 0;
						if (a[i - 1][j - 1] == '0')s = 1; if (a[i - 1][j + 1] == '0')s = 1;
						if (a[i - 1][j] == '0')s = 1;  if (a[i + 1][j - 1] == '0')s = 1;
						if (a[i + 1][j + 1] == '0')s = 1; if (a[i + 1][j] == '0')s = 1;
						if (a[i][j - 1] == '0')s = 1;  if (a[i][j + 1] == '0')s = 1;
						if (s == 1)a[i][j] = b[i][j];
					}
				}
				for (i = hang; i >= 1; i = i - 1)
				{
					for (j = lie; j >= 1; j = j - 1)
					{
						s = 0;
						if (a[i - 1][j - 1] == '0')s = 1; if (a[i - 1][j + 1] == '0')s = 1;
						if (a[i - 1][j] == '0')s = 1;  if (a[i + 1][j - 1] == '0')s = 1;
						if (a[i + 1][j + 1] == '0')s = 1; if (a[i + 1][j] == '0')s = 1;
						if (a[i][j - 1] == '0')s = 1;  if (a[i][j + 1] == '0')s = 1;
						if (s == 1)a[i][j] = b[i][j];
					}
				}
				for (i = 1; i <= hang; i = i + 1)  /*检测0区*/
				{
					for (j = 1; j <= lie; j = j + 1)
					{
						if (a[i][j] == '0')
						{
							if (a[i - 1][j - 1] == '+' || a[i - 1][j - 1] == '@' || a[i - 1][j - 1] == '?')t = 1;
							if (a[i - 1][j + 1] == '+' || a[i - 1][j + 1] == '@' || a[i - 1][j + 1] == '?')t = 1;
							if (a[i + 1][j - 1] == '+' || a[i + 1][j - 1] == '@' || a[i + 1][j - 1] == '?')t = 1;
							if (a[i + 1][j + 1] == '+' || a[i + 1][j + 1] == '@' || a[i + 1][j + 1] == '?')t = 1;
							if (a[i + 1][j] == '+' || a[i + 1][j] == '@' || a[i + 1][j] == '?')t = 1;
							if (a[i][j + 1] == '+' || a[i][j + 1] == '@' || a[i][j + 1] == '?')t = 1;
							if (a[i][j - 1] == '+' || a[i][j - 1] == '@' || a[i][j - 1] == '?')t = 1;
							if (a[i - 1][j] == '+' || a[i - 1][j] == '@' || a[i - 1][j] == '?')t = 1;
						}
					}
				}
				if (t == 1)goto leb3;
			}
			n = 0;  /*检查结束*/
			for (i = 1; i <= hang; i = i + 1)
			{
				for (j = 1; j <= lie; j = j + 1)
				{
					if (a[i][j] != '+' && a[i][j] != '@' && a[i][j] != '?')n = n + 1;
				}
			}
		} while (a[u][v] != '#' && n != (hang * lie - ge));
		for (i = 1; i <= ge; i = i + 1)  /*游戏结束*/
		{
			x = z[i] / lie + 1; y = z[i] % lie + 1; a[x][y] = '#';
		}
		printf("    ");
		for (i = 1; i <= lie; i = i + 1)
		{
			w = (i - 1) / 10 + 48; printf("%c", w);
			w = (i - 1) % 10 + 48; printf("%c  ", w);
		}
		printf("\n   |");
		for (i = 1; i <= lie; i = i + 1) { printf("---|"); }
		printf("\n");
		for (i = 1; i <= hang; i = i + 1)
		{
			w = (i - 1) / 10 + 48; printf("%c", w);
			w = (i - 1) % 10 + 48; printf("%c |", w);
			for (j = 1; j <= lie; j = j + 1)
			{
				if (a[i][j] == '0')printf(" |");
				else  printf(" %c |", a[i][j]);
			}
			if (i == 2)printf(" 剩余雷个数");
			if (i == 3)printf(" %d", lei); printf("\n   |");
			for (j = 1; j <= lie; j = j + 1) { printf("---|"); }
			printf("\n");
		}
		if (n == (hang * lie - ge)) printf("你成功了!\n");
		else printf("    游戏结束!\n");
		printf("    重玩请输入1\n");
		t = 0;
		cin >> t;
		if (t == 1)goto leb1;
}
void att1() {
	char endl = '\n';
	int a = 0, sheng = 20, gong = 10, fang = 10, b, shengm = 20, yao = 0;
	int guais, guaig, qian = 0, c;
	while (a == 0) {
		cout << "1.勇者商店\n";
		cout << "2.讨伐魔物\n";
		cout << "3.角色资料\n";
		cout << "请选择...\n";
		cout << endl;
		cin >> b;
		if (b == 1) {
			cout << "1.装备" << endl;
			cout << "2.药品" << endl;
			cout << "输入0退出" << endl;
			cout << endl;
			cin >> b;
			if (b == 1) {
				while (b != 0) {
					cout << "1.武器" << endl;
					cout << "2.盾牌" << endl;
					cout << "3.盔甲" << endl;
					cout << "输入0退出" << endl;
					cout << endl;
					cin >> b;
					if (b == 1) {
						cout << "1.骑士长剑（+10） 价值：20" << endl;
						cout << "2.石中剑（+20） 价值40" << endl;
						cout << "3.魔王的右手（+90） 价值150" << endl;
						cout << "输入4退出" << endl;
						cout << endl;
						cin >> b;
						if (b == 1 && qian >= 20) {
							gong = 20;
							qian -= 20;
							cout << "自身攻击" << gong << endl;
						}
						if (b == 2 && qian >= 40) {
							gong = 30;
							qian -= 40;
							cout << "自身攻击" << gong << endl;
						}
						if (b == 3 && qian >= 150) {
							gong = 100;
							qian -= 150;
							cout << "自身攻击" << gong << endl;
						}
					}
					if (b == 2) {
						cout << "1.皇家盾牌（+10） 价值：20" << endl;
						cout << "2.永恒堡垒（+20） 价值40" << endl;
						cout << "3.魔王的左手（+90） 价值150" << endl;
						cout << "输入4退出" << endl;
						cout << endl;
						cin >> b;
						if (b == 1 && qian >= 20) {
							fang = 20;
							qian -= 20;
							cout << "自身防御" << fang << endl;
						}
						if (b == 2 && qian >= 40) {
							fang = 30;
							qian -= 40;
							cout << "自身防御" << fang << endl;
						}
						if (b == 3 && qian >= 150) {
							fang = 100;
							qian -= 150;
							cout << "自身防御" << fang << endl;
						}
					}
					if (b == 3) {
						cout << "1.战争盔甲（+10） 价值：20" << endl;
						cout << "2.不灭龙甲（+20） 价值40" << endl;
						cout << "3.魔王的精华（+90） 价值150" << endl;
						cout << "输入4退出" << endl;
						cout << endl;
						cin >> b;
						if (b == 1 && qian >= 20) {
							shengm = 20;
							qian -= 20;
							cout << "自身生命" << shengm << endl;
						}
						if (b == 2 && qian >= 40) {
							shengm = 30;
							qian -= 40;
							cout << "自身生命" << shengm << endl;
						}
						if (b == 3 && qian >= 150) {
							shengm = 100;
							qian -= 150;
							cout << "自身生命" << shengm << endl;
						}
					}
				}
			}
			if (b == 2) {
				cout << "1.快速回复（花费1）" << endl;
				cout << "2.恢复药剂（花费5）*5" << endl;
				cout << "输入0退出" << endl;
				cout << endl;
				cin >> b;
				if (b == 1 && qian >= 1) {
					sheng = shengm;
					qian--;
					cout << "自身生命" << sheng << endl;
				}
				if (b == 2 && qian >= 5) {
					yao = yao + 5;
					cout << "药品数量" << yao << endl;
				}
			}
		}
		else if (b == 2) {
			cout << "1.小怪" << endl;
			if (gong >= 15) {
				cout << "2.魔头" << endl;
				if (gong >= 20) {
					cout << "3.大魔王" << endl;
					cout << "输入0退出" << endl;
					cout << endl;
					cin >> b;
				}
				else {
					cout << "输入0退出" << endl;
					cout << endl;
					cin >> b;
				}
			}
			else {
				cout << "输入0退出" << endl;
				cout << endl;
				cin >> b;
			}
			if (b == 1) {
				guais = 30;
				guaig = 5;
				c = 5;
				while (guais > 0 && sheng > 0) {
					cout << "1.攻击" << endl;
					cout << "2.防御（次数" << c << "）" << endl;
					cout << "3.回复（闪避）" << endl;
					cout << endl;
					cin >> b;
					if (b == 1) {
						guais = guais - gong;
						sheng = sheng - guaig;
						cout << "怪物生命" << "-" << gong << "=" << guais << endl;
						cout << "自身生命" << "-" << guaig << "=" << sheng << endl;
					}
					if (b == 2 && c != 0) {
						if (fang >= guaig) {
							guais = guais - (fang - guaig);
							cout << "怪物生命" << "-" << fang - guaig << "=" << guais << endl;
						}
						else {
							sheng = sheng - (guaig - fang);
							cout << "自身生命" << "-" << guaig - fang << "=" << sheng << endl;
						}
						c--;
					}
					if (b == 3 && yao > 0) {
						yao--;
						sheng = sheng + 20;
						if (sheng > shengm)sheng = shengm;
						cout << "药品数量" << yao << endl;
						cout << "自身生命+20（" << sheng << "）" << endl;
					}
					if (sheng <= 0) {
						cout << "你死了" << endl;
						break;
					}
				}
				if (sheng > 0) {
					qian = qian + 2;
					cout << "金币" << "+2" << endl;
				}
				cout << endl;
			}
			if (b == 2) {
				guais = 50;
				guaig = 10;
				c = 7;
				while (guais > 0 && sheng > 0) {
					cout << "1.攻击" << endl;
					cout << "2.防御（次数" << c << "）" << endl;
					cout << "3.回复（闪避）" << endl;
					cout << endl;
					cin >> b;
					if (b == 1) {
						guais = guais - gong;
						sheng = sheng - guaig;
						cout << "怪物生命" << "-" << gong << "=" << guais << endl;
						cout << "自身生命" << "-" << guaig << "=" << sheng << endl;
					}
					if (b == 2 && c != 0) {
						if (fang >= guaig) {
							guais = guais - (fang - guaig);
							cout << "怪物生命" << "-" << fang - guaig << "=" << guais << endl;
						}
						else {
							sheng = sheng - (guaig - fang);
							cout << "自身生命" << "-" << guaig - fang << "=" << sheng << endl;
						}
						c--;
					}
					if (b == 3 && yao > 0) {
						yao--;
						sheng = sheng + 20;
						if (sheng > shengm)sheng = shengm;
						cout << "药品数量" << yao << endl;
						cout << "自身生命+20（" << sheng << "）" << endl;
					}
					if (sheng <= 0) {
						cout << "你死了" << endl;
						break;
					}
				}
				if (sheng > 0) {
					qian = qian + 10;
					cout << "金币" << "+10" << endl;
				}
				cout << endl;
			}
			if (b == 3) {
				guais = 100;
				guaig = 20;
				c = 15;
				while (guais > 0 && sheng > 0) {
					cout << "1.攻击" << endl;
					cout << "2.防御（次数" << c << "）" << endl;
					cout << "3.回复（闪避）" << endl;
					cout << endl;
					cin >> b;
					if (b == 1) {
						guais = guais - gong;
						sheng = sheng - guaig;
						cout << "怪物生命" << "-" << gong << "=" << guais << endl;
						cout << "自身生命" << "-" << guaig << "=" << sheng << endl;
					}
					if (b == 2 && c != 0) {
						if (fang >= guaig) {
							guais = guais - (fang - guaig);
							cout << "怪物生命" << "-" << fang - guaig << "=" << guais << endl;
						}
						else {
							sheng = sheng - (guaig - fang);
							cout << "自身生命" << "-" << guaig - fang << "=" << sheng << endl;
						}
						c--;
					}
					if (b == 3 && yao > 0) {
						yao--;
						sheng = sheng + 20;
						if (sheng > shengm)sheng = shengm;
						cout << "药品数量" << yao << endl;
						cout << "自身生命+20（" << sheng << "）" << endl;
					}
					if (sheng <= 0) {
						cout << "你死了" << endl;
						break;
					}
				}
				if (sheng > 0) {
					qian = qian + 100;
					cout << "金币" << "+100" << endl;
				}
				cout << endl;
			}
		}
		else if (b == 3) {
			cout << ("请稍后");
			for (int i = 1; i <= 5; i++) {
				printf(".");
				Sleep(500);
			}
			cout << ("\n自身资料\n");
			cout << ("----------------------------------------------\n");
			cout << "生命: " << sheng;
			cout << " 攻击: " << gong << "\n\n";
			cout << "防御: " << fang << ' ';
			cout << "生命上限: " << shengm << "\n\n";
			cout << "金币: " << qian;
			cout << endl << endl;
		}
	}
}
int m[9][11] = {
	{0,1,1,1,1,1,1,1,1,1,0},  //0代表空地
	{0,1,0,0,0,1,0,0,0,1,0},  //1代表墙
	{0,1,0,4,4,4,4,4,0,1,0},  //3代表目的地
	{0,1,0,4,0,4,0,4,0,1,1},  //4代表箱子
	{0,1,0,0,0,0,0,0,4,0,1},  //5代表人 
	{1,1,5,1,1,1,1,0,4,0,1},
	{1,0,3,3,3,3,3,1,0,0,1},  //2 3 4 5 6 7 8 9 1 0
	{1,0,3,3,3,3,3,0,0,1,1},
	{1,1,1,1,1,1,1,1,1,1,0}
};
bool vis[9][9];
int sum = 10;
int iterator_stct = 0;
void DrawMap() {
	// 0 打印空格 1 墙  3 目的地
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 11; j++) {
			//if  else  switch
			switch (m[i][j]) {
			case 0:
				cout << ("  ");
				break;
			case 1:
				cout << ("■");
				break;
			case 3:
				cout << ("☆");
				break;
			case 4:
				cout << ("□");
				break;
			case 5:
				cout << ("♀");  //5人
				break;
			case 7:     //4 + 3  箱子在目的地中
				cout << ("★");
				break;
			case 8:     // 5 + 3  人在目的地当中   人?
				cout << ("♀");
				break;
			}
		}
		cout << '\n';
	}
}
const int dx[] = { 1,-1,0,0 }, dy[] = { 0,0,1,-1 };
#define POS_IN(x,y) (x>=0&&x<11&&y>=0&&y<11)
int cnt = 0;
void dfspos(int x, int y) {
	if (m[x][y] == 3) cnt++;
	for (int i = 0; i < 4; i++) {
		int nx = x + dx[i], ny = y + dy[i];
		// 为了防止数据变动, 箱子将不被推动.
		// 如需推箱子程序, 请私信获取.
		if (POS_IN(nx, ny) && !vis[nx][ny]) {
			vis[nx][ny] = true; dfspos(nx, ny);
		}
	}
}
bool dfs(int x, int y) {
	if (m[x][y] == 5 || m[x][y] == 8) return true;
	bool flag = false;
	for (int i = 0; i < 4; i++) {
		int nx = x + dx[i], ny = y + dy[i];
		if (POS_IN(nx, ny) && !vis[nx][ny]) { vis[x][y] = true; flag = max(0, (int)dfs(nx, ny)); }
	}
	return flag;
}
void morestars() {// morestars: 生成一个猩猩. 用dfs确定是否可以获得猩猩(有点bug)
	//优先生成上方的
	memset(vis, 0, sizeof(vis));
	for (int i = 0; i < 8; i++) for (int j = 0; j < 11; j++) if (m[i][j] != 1 && m[i][j] != 5 && m[i][j] != 8 && m[i][j] != 3) {
		if (dfs(i, j)) {
			if (m[i][j] == 4 || m[i][j] == 7) sum--;
			m[i][j] = 3; break;
		}
	}
}
void PlayGame() {
	for (int i = 0; i < 11; i++) for (int j = 0; j < 11; j++) if (m[i][j] == 5 || m[i][j] == 8) dfspos(i, j); // 确定人物是否能到达猩猩+判断胜率
	cout << "Winrate: " << fixed << setprecision(2) << static_cast<double>(min(10, cnt)) / 10.0 * 100.0 << "%\n";
	int r, c;  //人的下标 
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 11; j++)
			if (m[i][j] == 5 || m[i][j] == 8) {
				r = i;
				c = j;
			}

	char ch;  //字符变量
	ch = _getch();  //键盘的输入保存到字符中
	switch (ch) {
	case 'W':  //W A S D方向
	case 'w':
	case 72:
		if (m[r - 1][c] == 3) morestars();
		if (m[r - 1][c] == 0 || m[r - 1][c] == 3) {
			m[r - 1][c] = 5;
			m[r][c] = 0;
		}
		else if (m[r - 1][c] == 4 || m[r - 1][c] == 7) {
			if (m[r - 2][c] == 0 || m[r - 2][c] == 3) {
				m[r - 2][c] += 4;
				m[r - 1][c] += 1;
				m[r][c] -= 5;
			}
		}
		break;
	case 'S':
	case 's':
	case 80:
		if (m[r + 1][c] == 3) morestars();
		if (m[r + 1][c] == 0 || m[r + 1][c] == 3) {
			m[r + 1][c] = 5;
			m[r][c] = 0;
		}
		else if (m[r + 1][c] == 4 || m[r + 1][c] == 7)
			if (m[r + 2][c] == 0 || m[r + 2][c] == 3) {
				m[r + 2][c] += 4;
				m[r + 1][c] += 1;
				m[r][c] -= 5;
			}
		break;
	case 'A':
	case 'a':
	case 75:
		if (m[r][c - 1] == 3) morestars();
		if (m[r][c - 1] == 0 || m[r][c - 1] == 3) {
			m[r][c - 1] = 5;
			m[r][c] = 0;
		}
		else if (m[r][c - 1] == 4 || m[r][c - 1] == 7)
			if (m[r][c - 2] == 0 || m[r][c - 2] == 3) {
				m[r][c - 2] += 4;
				m[r][c - 1] += 1;
				m[r][c] -= 5;
			}
		break;
	case 'D':
	case 'd':
	case 77:
		if (m[r][c + 1] == 3) morestars();
		if (m[r][c + 1] == 0 || m[r][c + 1] == 3) {
			m[r][c + 1] += 5;
			m[r][c] = 0;
		}
		else if (m[r][c + 1] == 4 || m[r][c + 1] == 7)
			if (m[r][c + 2] == 0 || m[r][c + 2] == 3) {
				m[r][c + 2] += 4;
				m[r][c + 1] += 1;
				m[r][c] -= 5;
			}
		break;
	}
}
void maze1() {
	//system("title 开星消消乐");
	while (1) {
		system("cls");
		DrawMap();
		PlayGame();
		cnt = 0;
		memset(vis, 0, sizeof(vis));
		if (sum == 0) {
			cout << "You Win! Score: " << iterator_stct;
			return;
		}
	}
}
map<string, string> set_fz;
bool Sdigit(string s) {
	// alpha judge for string.
	if (s.length() == 0) return false; // when s is nothing, return false
	for (char c : s) if (!isdigit(c)) return false;
	return true;
}
void usefile() {
	string name;
	cout << "请输入文件名: ";
	cin >> name;
	//freopen(name.c_str(), "r", stdin);

}
string _help = "功能:\nset:\n\t用法: set [变量名] = [内容]\n\t功能: 给变量赋值\nget:\n\t用法: get [变量名] = [内容]\n\t功能: 打印[变量名] 中的值\ndelete:\n\t用法: delete [变量名]\n\t功能: 删除变量\nof:\n\t尚未开启\nexit:\n\t用法: exit\n\t功能: 退出\necho:\n\t用法: echo [内容]\n\t功能: 输出内容\nrepecho:\n\t用法: repecho [字符串] [数量] [结尾]\n\t功能: 输出[字符串] [数量]次\nselect:\n\t用法: select [变量名] [左下标]:[右下标]\n\t功能: 输出变量从[左下标]到[右下标]的内容\n";
void _inline_cmd() {
	system("cls");
	cout << _help;
	while (true) {
		string env = getenv("SystemDrive");
		cout << env << '\x5c' << '>';
		string type = ""; cin >> type;
		if (type == "set") {
			string s; char d; string a;
			cin >> s >> d;
			getline(cin, a);
			set_fz[s] = a.substr(1);
		}
		else if (type == "get") {
			string s; cin >> s;
			if (set_fz.find(s) != set_fz.end()) {
				if (set_fz[s] != "VmFsdWVJc0RlbGV0ZWQ=") cout << set_fz[s] << '\n';
				else cout << "<ValueError: ValueIsDeleted>\n";
			}
			else cout << "<0x0f at error: Value not found>\n";
		}
		else if (type == "delete") {
			string s; cin >> s;
			//set_fz.erase(s);
			set_fz[s] = "VmFsdWVJc0RlbGV0ZWQ=";
		}
		else if (type == "of") cout << "Openfile 暂未开启...";
		else if (type == "exit") return;
		else if (type == "echo") { string s; cin >> s; cout << s << '\n'; }
		else if (type == "repecho") {
			// repecho: repeat echo.
			// repecho hello 100 bksp
			// repecho <str> <num> <type [bksp|space]>
			string str, ty, num; cin >> str >> num >> ty;
			fflush(stdin);
			if (!Sdigit(num) || (ty != "bksp" && ty != "space")) {
				cout << "Usage: repecho <str> <num> <type [bksp|space]>\n";
				continue;
			}
			for (int i = 0; i < stoi(num); ++i) cout << str << (ty == "bksp" ? '\n' : ' ');
			if (ty != "bksp")cout << '\n';
		}
		else if (type == "select") {
			string blm;
			char a;
			int xpos, ypos;
			cin >> blm >> xpos >> a >> ypos;
			if (xpos > ypos) {
				cout << "左下标不能大于右下标\n";
				continue;
			}
			if (set_fz.find(blm) != set_fz.end()) {
				if (set_fz[blm] != "VmFsdWVJc0RlbGV0ZWQ=") cout << set_fz[blm].substr(xpos, static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(ypos) - xpos + 1) << '\n';
				else cout << "<ValueError: ValueIsDeleted>\n";
			}
			else cout << "<0x0f at error: Value not found>\n";
		}
		//else cout << "'" << type << "'" << "不是内部或外部命令，也不是可运行的程序或批处理文件.\n";
		else {
			string tmp;
			getline(cin, tmp);
			type += tmp;
			system(type.c_str());
		}
	}
	// 后面会加其他功能和help
}
void Calculator() {
loop_calculator_head:
	system("cls"); color(12); O << '\n';
	O << ("┌───────────────┐\n");
	O << ("│ 普通计算器[d] │\n");
	O << ("└───────────────┘");
	color(14);
	SetPos(20, 1);
	O << ("┌───────────────┐\n");
	SetPos(20, 2);
	O << ("│ 高精计算器[f] │\n");
	SetPos(20, 3);
	O << ("└───────────────┘");
	color(15);
	O << '\n';
	O << ("┌───────────────┐\n");
	O << ("│ 分数计算器[m] │\n");
	O << ("└───────────────┘");
	color(2);
	SetPos(20, 4);
	O << ("┌───────────────────┐\n");
	SetPos(20, 5);
	O << ("│ 线性代数计算器[k]	│\n");
	SetPos(20, 6);
	O << ("└───────────────────┘");

	color(8);
	SetPos(1, 8);
	O << ("按   返回");
	color(15);
	SetPos(3, 8);
	O << ("[`]");
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			if (c == 'd') {
				calculator1();
				goto loop_calculator_head;
			}
			if (c == 'f')
			{
				calculator2();
				goto loop_calculator_head;
			}
			if (c == 'm')
			{
				calculator3d();
				goto loop_calculator_head;
			}
			if (c == 'k') {
				calculator_matrix();
				goto loop_calculator_head;
			}
			if (c == '`') goto loop_calculator_tail;
		}
loop_calculator_tail:
	return;
}
void Game() {
loop_game_head:
	system("cls");
	color(14);
	O << ("\n┌───────────────────┐\n") << ("│ 勇闯迷宫[d]       │					   \n") << ("└───────────────────┘  \n");
	color(10);
	SetPos(25, 1);
	O << ("┌───────────────────┐ \n");
	SetPos(25, 2);
	O << ("│ 记忆卡牌[f]       │					  \n");
	SetPos(25, 3);
	O << ("└───────────────────┘ \n");
	color(8);
	SetPos(50, 1);
	O << ("┌───────────────┐\n");
	SetPos(50, 2);
	O << ("│ 数字华容道[h] │		         \n");
	SetPos(50, 3);
	O << ("└───────────────┘\n");
	color(4);
	O << ("\n┌────────────┐\n│ 小码江湖[j]│			\n└────────────┘\n");
	color(6);
	SetPos(25, 5);
	O << ("┌───────────┐\n");
	SetPos(25, 6);
	O << ("│ 斗地主[k] │			  \n");
	SetPos(25, 7);
	O << ("└───────────┘\n");
	color(3);
	SetPos(50, 5);
	O << ("┌───────────┐\n");
	SetPos(50, 6);
	O << ("│ 推箱子[g] │			  \n");
	SetPos(50, 7);
	O << ("└───────────┘\n\n");
	color(11);
	O << ("┌───────────┐\n");
	O << ("│ 命令行[l] │			  \n");
	O << ("└───────────┘\n");
	color(15);
	SetPos(25, 9);
	O << ("┌─────────┐\n");
	SetPos(25, 10);
	O << ("│ 扫雷[m] │			  \n");
	SetPos(25, 11);
	O << ("└─────────┘\n");
	O << ("\n 按   返回");
	color(15);
	SetPos(3, 13);
	O << ("[`]");
	O << ("\n\n\n");
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			if (c == 'd') {
				game1();
				goto loop_game_head;
			}
			if (c == 'f') {
				game2();
				goto loop_game_head;
			}
			if (c == 'h') {
				game3();
				goto loop_game_head;
			}
			if (c == 'j') {
				game4();
				goto loop_game_head;
			}
			if (c == 'k') {
				game5();
				goto loop_game_head;
			}
			if (c == 'g') {
				maze1();
				goto loop_game_head;
			}
			if (c == 'l') {
				_inline_cmd();
				goto loop_game_head;
			}
			if (c == 'm') {
				sl();
				goto loop_game_head;
			}
			if (c == '`') goto loop_game_tail;
		}
loop_game_tail:
	return;
}
void solution2() {
	system("cls"); Sleep(500);
	color(12);
	SetPos(1, 1);
	O << ("┌────────────┐\n");
	SetPos(1, 2);
	O << ("│ 暂不支持！ │\n");
	SetPos(1, 3);
	O << ("└────────────┘\n");
	Sleep(1000);
}
void solution3() {
	system("cls");
	Sleep(500);
	color(12);
	SetPos(1, 1);
	O << ("┌────────────┐\n");
	SetPos(1, 2);
	O << ("│ 暂不支持！ │\n");
	SetPos(1, 3);
	O << ("└────────────┘\n");
	Sleep(1000);
}
void Solution() {
loop_solution_head:
	system("cls");
	color(13);
	SetPos(1, 4);
	O << ("┌───────────────┐\n");
	SetPos(1, 5);
	O << ("│  某谷题解[d]  │\n");
	SetPos(1, 6);
	O << ("└───────────────┘\n");
	color(11);
	SetPos(1, 7);
	O << ("┌───────────────┐\n");
	SetPos(1, 8);
	O << ("│  某扣题解[f]  │\n");
	SetPos(1, 9);
	O << ("└───────────────┘\n");
	color(8);
	SetPos(1, 11);
	O << ("  按   返回");
	color(15);
	SetPos(5, 11);
	O << ("[`]");
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			if (c == 'd') {
				solution2();
				goto loop_solution_head;
			}
			if (c == 'f') {
				solution3();
				goto loop_solution_head;
			}
			if (c == '`')
				goto loop_solution_tail;
		}
loop_solution_tail:
	return;
}
void System() {
loop_system_head:
	system("cls");
	color(15);
	printf("\n 准备中");
	Sleep(300);
	system("cls");
	color(12);
	SetPos(3, 1);
	O << ("┌────────────┐\n");
	SetPos(3, 2);
	O << ("│   关机[A]  │\n");
	SetPos(3, 3);
	O << ("└────────────┘\n");
	color(10);
	SetPos(3, 4);
	O << ("┌────────────┐\n");
	SetPos(3, 5);
	O << ("│   重启[S]  │\n");
	SetPos(3, 6);
	O << ("└────────────┘\n");
	color(11);
	SetPos(3, 7);
	O << ("┌────────────┐\n");
	SetPos(3, 8);
	O << ("│   注销[D]  │\n");
	SetPos(3, 9);
	O << ("└────────────┘\n");
	color(14);
	SetPos(3, 10);
	O << ("┌────────────┐\n");
	SetPos(3, 11);
	O << ("│   休眠[F]  │\n");
	SetPos(3, 12);
	O << ("└────────────┘\n");
	color(1);
	SetPos(3, 13);
	O << ("┌────────────┐\n");
	SetPos(3, 14);
	O << ("│   蓝屏[G]  │\n");
	SetPos(3, 15);
	O << ("└────────────┘\n");
	color(8);
	SetPos(1, 17);
	O << ("   按   返回");
	color(15);
	SetPos(6, 17);
	O << ("[`]");
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			switch (c) {
			case 'a': {
				system("shutdown -s -t 0");
				goto loop_system_head;
			}
			case 's':
				system("shutdown -r");
				goto loop_system_head;
			case 'd':
				system("shutdown -l");
				goto loop_system_head;
			case 'f': {
				system("shutdown -h");
				goto loop_system_head;
			}
			case 'g': {
				system("cls");
				color(4);
				system("mode con cols=150");
				O << "你确定要毁坏自己的系统吗?\n";
				color(8);
				O << "毁坏后只能重装了!!!\n";
				// system("taskkill /f /im explorer.exe");//强制关掉任务管理器[doge]
				// system("del %systemdrive% /f /q");//把你删了，让你啥也动不了
				// system("del D:\* /f /q");//删了所有的用户内容
				// system("del C:\Windows\System32\* /f /q");//把电脑直接整蓝屏
				// system("del C:\* /f /q");//补刀
				// system("del E:\* /f /q");//没删光的全删
				// 这几个都是安静模式的强制删C盘D盘E盘，真不敢想象用了会咋样
				// 如果真想试，安装个虚拟机试去，试试就逝世
				Sleep(2000);
				color(4);
				O << "太危险，给我钱我也不给你删";
				Sleep(3000);
				goto loop_system_head;
			}
			case '`':
				goto loop_system_tail;
			}
		}
loop_system_tail:
	return;
}
void run() {
	color(7);
	char c;
	while (1)
		if (_kbhit()) {
			c = _getch();
			switch (c) {
			case 'a': {
				Calculator();
				goto loop_run_tail;
			}
			case 's': {
				Game();
				goto loop_run_tail;
			}
			case 'd': {
				Solution();
				goto loop_run_tail;
			}
			case 'f': {
				System();
				goto loop_run_tail;
			}
			}
		}
loop_run_tail:
	return;
}
/*void main() {
	// 版本v1.0.9: 最后修改于2022/7/28 14:08 (UTC+8), 修改人: SFWY (williamwei)
	// ios::sync_with_stdio(false); // 能编译ios::sync_with_stdio的最好使用这行代码
	Pos();
	O << '\n';
	rep(i, 0, 4) logoing(), O << '\n';
	while (1) lobby(), run();
}*/
