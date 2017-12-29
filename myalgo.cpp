#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>

const int hupai_hushu = 12;//胡牌所需最低点数

const int hushu_eat_hei_3 = 1;		//黑色坎子（对牌形成）
const int hushu_own_hei_3 = 3;		//黑色坎子（原手持有）
const int hushu_eat_hei_4 = 6;		//黑色滚子（开牌形成）
const int hushu_own_hei_4 = 9;		//黑色开水（原手持有）
const int hushu_both_hei_2710 = 3;	//黑色二七十（不区分原手与非原手）

const int hushu_eat_hong_3 = 3;		//红色坎子（对牌形成）
const int hushu_own_hong_3 = 6;		//红色坎子（原手持有）
const int hushu_eat_hong_4 = 9;		//红色滚子（开牌形成）
const int hushu_own_hong_4 = 12;	//红色开水（原手持有）
const int hushu_both_hong_2710 = 6;	//红色二七十（不区分原手与非原手）

									//这几种不算胡数
const int hushu_hei_lianzi = 0;				//黑色连子
const int hushu_hong_lianzi = 0;			//红色连子
const int hushu_heiheihong_banbianzi = 0;	//黑黑红 半边字
const int hushu_honghonghei_banbianzi = 0;	//红红黑 半边字

typedef struct step
{
	int curr_try_card;	   //尝试的牌，1-20 1-10对应黑牌1-10,11-20对应红牌1-10
	int curr_try_type;	  //尝试对牌拆分顺子，半边子（红红黑，黑黑红），二七十 这4种类型
	int curr_hushu;
	int curr_is_ok;
}STEP;



//函数返回值-1说明不能胡（有某些牌不能都成房）;
//返回值大于等于0说明可能胡牌（所有牌能成房）,并且这个胡数是所有拆牌可能里胡数最大的那个
int __get_hushu_no_gun_kan(int cards[21])//第0位没用，为了和1-10对应起来方便看
{
	STEP steps[7];
	memset(steps, 0, sizeof(STEP) * 7);

	int max_hushu = -1;
	int curr_step = -1;
	int curr_try_card = 1;
	int curr_type_finded = 1;

	while (true)
	{
	label_next_type:
		if (curr_type_finded)
		{
			curr_type_finded = 0;
			int has_next_card = 0;
			//尝试从当前步骤的curr_try_card到20每一张牌
			for (int i = curr_try_card; i <= 20; i++)
			{
				if (cards[i] > 0)
				{
					curr_try_card = i;
					curr_step++;
					steps[curr_step].curr_try_card = curr_try_card;
					steps[curr_step].curr_hushu = -1;

					has_next_card = 1;
					break;
				}
			}
			//没有其他牌了,到了搜索的最深处。一路上所有的都组成了正确的牌型。寻找最大的胡数
			if (!has_next_card)
			{
				int all_hushu = 0;
				for (int i = 0; i <= curr_step; ++i)
					all_hushu += steps[i].curr_hushu;
				if (all_hushu > max_hushu)
					max_hushu = all_hushu;
				//回溯到上一层
				goto flag_huishu;
			}
		}
		//黑色顺子
		if (steps[curr_step].curr_try_type == 0)//第一次进入新状态
		{
			steps[curr_step].curr_try_type = 11;
			if (curr_try_card <= 8)//
			{
				if (cards[curr_try_card + 1] > 0 && cards[curr_try_card + 2] > 0)//有黑顺
				{
					steps[curr_step].curr_try_card = curr_try_card;
					steps[curr_step].curr_hushu = hushu_hei_lianzi;//黑顺胡数 
					steps[curr_step].curr_is_ok = 1;
					curr_type_finded = 1;

					cards[curr_try_card] -= 1;
					cards[curr_try_card + 1] -= 1;
					cards[curr_try_card + 2] -= 1;
					goto label_next_type;
				}
			}
		}
		//黑色半边字:黑红红
		if (steps[curr_step].curr_try_type == 11)
		{
			//红色半边字
			steps[curr_step].curr_try_type = 12;
			//检查有没有黑色半边字 黑红红
			if (curr_try_card <= 10 && cards[curr_try_card] >= 1 && cards[curr_try_card + 10] >= 2)
			{
				steps[curr_step].curr_try_card = curr_try_card;
				steps[curr_step].curr_hushu = hushu_honghonghei_banbianzi;
				steps[curr_step].curr_is_ok = 1;
				curr_type_finded = true;

				cards[curr_try_card] -= 1;
				cards[curr_try_card + 10] -= 2;
				goto label_next_type;
			}
		}
		//黑色半边字:黑黑红
		if (steps[curr_step].curr_try_type == 12)
		{
			steps[curr_step].curr_try_type = 13;
			//检查有没有黑色半边字 黑黑红
			if (curr_try_card <= 10 && cards[curr_try_card] >= 2 && cards[curr_try_card + 10] >= 1)
			{
				steps[curr_step].curr_try_card = curr_try_card;
				steps[curr_step].curr_hushu = hushu_heiheihong_banbianzi;
				steps[curr_step].curr_is_ok = 1;
				curr_type_finded = true;

				cards[curr_try_card] -= 2;
				cards[curr_try_card + 10] -= 1;
				goto label_next_type;
			}
		}

		//黑色2710
		if (steps[curr_step].curr_try_type == 13)
		{
			steps[curr_step].curr_try_type = 14;
			if (curr_try_card <= 10 && curr_try_card == 2 && cards[7] > 0 && cards[10] > 0)
			{
				steps[curr_step].curr_try_card = 2;
				steps[curr_step].curr_hushu = hushu_both_hei_2710;
				steps[curr_step].curr_is_ok = 1;
				curr_type_finded = true;

				cards[2] -= 1;
				cards[7] -= 1;
				cards[10] -= 1;
				goto label_next_type;
			}
		}

		//红色顺子
		if (steps[curr_step].curr_try_type == 14) {
			steps[curr_step].curr_try_type = 21;
			if (curr_try_card > 10 && curr_try_card <= 18)
			{
				if (cards[curr_try_card + 1] > 0 && cards[curr_try_card + 2] > 0)
				{
					steps[curr_step].curr_try_card = curr_try_card;
					steps[curr_step].curr_hushu = hushu_hong_lianzi;
					steps[curr_step].curr_is_ok = 1;
					curr_type_finded = true;

					cards[curr_try_card] -= 1;
					cards[curr_try_card + 1] -= 1;
					cards[curr_try_card + 2] -= 1;
					goto label_next_type;
				}
			}
		}
		//红色半边字，红黑黑
		if (steps[curr_step].curr_try_type == 21)
		{
			steps[curr_step].curr_try_type = 22;
			if (curr_try_card > 10 && cards[curr_try_card] >= 1 && cards[curr_try_card - 10] >= 2)
			{
				steps[curr_step].curr_try_card = curr_try_card;
				steps[curr_step].curr_hushu = hushu_heiheihong_banbianzi;
				steps[curr_step].curr_is_ok = 1;
				curr_type_finded = true;
				cards[curr_try_card] -= 1;
				cards[curr_try_card - 10] -= 2;
				goto label_next_type;
			}
		}
		//红色半边字，红红黑
		if (steps[curr_step].curr_try_type == 22)
		{
			steps[curr_step].curr_try_type = 23;
			if (curr_try_card > 10 && cards[curr_try_card] >= 2 && cards[curr_try_card - 10] >= 1)
			{
				steps[curr_step].curr_try_card = curr_try_card;
				steps[curr_step].curr_hushu = hushu_honghonghei_banbianzi;
				steps[curr_step].curr_is_ok = 1;
				curr_type_finded = true;
				cards[curr_try_card] -= 2;
				cards[curr_try_card - 10] -= 1;
				goto label_next_type;
			}
		}
		//红色2710
		if (steps[curr_step].curr_try_type == 23)
		{
			steps[curr_step].curr_try_type = 24;
			if (curr_try_card == 12 && cards[17] > 0 && cards[20] > 0)
			{
				steps[curr_step].curr_try_card = 12;
				steps[curr_step].curr_hushu = hushu_both_hong_2710;
				steps[curr_step].curr_is_ok = 1;
				curr_type_finded = true;

				cards[12] -= 1;
				cards[17] -= 1;
				cards[20] -= 1;
				goto label_next_type;
			}
		}

		//检查是否满足条件，条件是指当前步骤curr_step里的curr_try_card的对应curr_try_type的分割类型curr_is_ok是true
	flag_huishu:
		if (curr_step < 0)
			goto finish;
		if (1 != steps[curr_step].curr_is_ok || steps[curr_step].curr_try_type == 24)//裁剪不正确的（23表示最后一个情况都判断完了）
		{
			memset(&steps[curr_step], 0, sizeof(STEP));
			curr_step--;
			goto flag_huishu;
		}
		//找到了合法的牌型
		curr_try_card = steps[curr_step].curr_try_card;
		int tp = steps[curr_step].curr_try_type;
		if (tp == 11 || tp == 21)//黑色或红色顺子
		{
			cards[curr_try_card] += 1;
			cards[curr_try_card + 1] += 1;
			cards[curr_try_card + 2] += 1;
		}
		else if (tp == 12)//黑色：黑红红半边字
		{
			cards[curr_try_card] += 1;
			cards[curr_try_card + 10] += 2;
		}
		else if (tp == 13)//黑色：黑黑红半边字
		{
			cards[curr_try_card] += 2;
			cards[curr_try_card + 10] += 1;
		}
		else if (tp == 22)//红色:红黑黑半边字
		{
			cards[curr_try_card] += 1;
			cards[curr_try_card - 10] += 2;
		}
		else if (tp == 23)//红色:红红黑半边字
		{
			cards[curr_try_card] += 2;
			cards[curr_try_card - 10] += 1;
		}
		else if (tp == 14 || tp == 24)//黑色或红色2,7,10
		{
			if (curr_try_card != 2 && curr_try_card != 22)
			{
				//__asm{ int 3 };
			}
			cards[2] += 1;
			cards[7] += 1;
			cards[10] += 1;
			////最后一种情况：红色2710，需要回退
			//if (tp == 24)
			//{
			//	curr_step--;
			//	if (curr_step == 0)
			//		goto finish;
			//	goto flag_huishu;
			//}

		}
	}

finish:
	return max_hushu;
}


//获得手里牌的胡数,表示标记 cards 里等于curr_eat_card 的是吃的牌，胡牌时候可以组成对子。curr_eat_card等于1-20
int get_hushu(int cards[21], int curr_eat_card = 0)
{
	int gunzi_kanzi_hushu = 0;
	for (int i = 1; i <= 10; ++i) {
		if (cards[i] == 4) //黑色开水（原手持有）
		{
			gunzi_kanzi_hushu += hushu_own_hei_4;
			cards[i] = 0;
		}
		if (cards[i] == 3)//黑色坎子（原手持有）	
		{
			gunzi_kanzi_hushu += hushu_own_hei_3;
			cards[i] = 0;
		}
	}

	for (int i = 11; i <= 20; ++i) {
		if (cards[i] == 4) //红色开水（原手持有）
		{
			gunzi_kanzi_hushu += hushu_own_hong_4;
			cards[i] = 0;
		}
		if (cards[i] == 3)//红色坎子（原手持有）	
		{
			gunzi_kanzi_hushu += hushu_own_hong_3;
			cards[i] = 0;
		}
	}
	//统计牌的数量，现在只剩下每张牌最多2张了。
	int two_num = 0;//统计对子数量
	for (int i = 1; i <= 20; i++) {
		if (2 == cards[i] && i != curr_eat_card)
			two_num += 1;
	}
	int total_hushu = gunzi_kanzi_hushu;
	///////////////////////////////////////////////////////////
	/*int other_hushu = -1;
	//手中cards里这个牌是吃的，才允许组成单独的对子成房
	if (curr_eat_card !=0)
	{
	int other_hushu1 = -1;
	int other_hushu2 = -1;
	if (cards[curr_eat_card] == 2)//吃这个牌可能组成的是单独对子,判断下胡数
	{
	cards[curr_eat_card] = 0;
	other_hushu1 = __get_hushu_no_gun_kan(cards);
	cards[curr_eat_card] = 2;//还原
	}
	else
	{
	other_hushu2 = __get_hushu_no_gun_kan(cards);
	}
	other_hushu = other_hushu1 > other_hushu2 ? other_hushu1 : other_hushu2;
	}
	else//手上的牌不允许有对子单独存在
	{
	other_hushu = __get_hushu_no_gun_kan(cards);
	}
	if (other_hushu < 0)
	return -1;
	else
	return total_hushu + other_hushu;*/
	/////////////////////////////////////////////////////
	/*// 不带对子。都是顺子，半边字，二七十等三张组成（如果都能组成房）
	if (sum % 3 == 0) {
	// 除坎以外的顺子的胡息
	int other_hushu = __get_hushu_no_gun_kan(cards);
	if (other_hushu < 0) //不能组成房
	return -1;
	return total_hushu + other_hushu;
	}
	else {
	// 带对子,尝试去掉每个对子，判断剩下的。平时普通对子不能出现，只有胡牌时候吃成的对子才能单独存在
	int max_hushu = -1;
	int tmp_cards[21];
	for (int i = 1; i <= 20; i++) {
	if (cards[i] == 2 && curr_eat_card == i)
	{
	memcpy(tmp_cards, cards, sizeof(tmp_cards));
	tmp_cards[i] = 0;
	int other_hushu = __get_hushu_no_gun_kan(tmp_cards);//这里面都匹配完成不会存在单独的对子
	if (other_hushu >= 0) {
	if (other_hushu > max_hushu)
	max_hushu = other_hushu;
	}
	}
	}
	if (max_hushu < 0)//都尝试完所有的对子，都没成房，说明不能胡
	return -1;
	return 	total_hushu + max_hushu;
	}
	*/
	// 当前吃的牌之外不带对子。都是顺子，半边字，二七十等三张组成（如果都能组成房）
	if (two_num == 0) {
		// 除坎以外的顺子的胡息
		int other_hushu = __get_hushu_no_gun_kan(cards);
		if (other_hushu < 0) //不能组成房
			return -1;
		return total_hushu + other_hushu;
	}
	else {
		// 当前吃的牌之外带对子,尝试去掉每个对子，判断剩下的。平时普通对子不能出现，只有胡牌时候才能有且仅有对子才能单独存在
		int max_hushu = -1;
		int tmp_cards[21];
		for (int i = 1; i <= 20; i++) {
			if (cards[i] == 2 && i != curr_eat_card)// 去掉其他对子，这个刚“吃掉”的对子必须能够成房
			{
				memcpy(tmp_cards, cards, sizeof(tmp_cards));
				tmp_cards[i] = 0;
				int other_hushu = __get_hushu_no_gun_kan(tmp_cards);
				if (other_hushu >= 0) {
					if (other_hushu > max_hushu)
						max_hushu = other_hushu;
				}
			}
		}
		if (max_hushu < 0)//都尝试完所有的对子，都没成房，说明不能胡
			return -1;
		return 	total_hushu + max_hushu;
	}

}


typedef struct {
	int can_hupai;		//吃了能否胡牌
	int hushu;
	int eat_count;		//能吃的牌型数量
	int eat_with[6][4];	//能被哪些牌型吃
	int eat_kind;	//能否吃，是否需要摆牌
}EatResult;
//  2 3 4 5 6 7 8
//i   x x x		连子
//i		x x x	连子
//i		  x	x x 连子
//		  xx	吃成坎子
//		  XX	半边字
//		 2710	二七十

//吃牌类型
//红色
const int EatTo_hong_4 = 1;
const int EatTo_hong_3 = 2;
const int EatTo_hong_2710 = 3;
const int EatTo_hong_lianzi = 4;
const int EatTo_hong_banbianzi = 5;//红红黑
const int EatTo_hong_duizi = 6;
//黑色
const int EatTo_hei_4 = 7;
const int EatTo_hei_3 = 8;
const int EatTo_hei_2710 = 9;
const int EatTo_hei_lianzi = 10;
const int EatTo_hei_banbianzi = 11;//黑黑红
const int EatTo_hei_duizi = 12;

const char * EatName[13] = {
	NULL,
	"红 开水",
	"红 坎子",
	"红 2710",
	"红 连子",
	"红 半边字",
	"红 对子",

	"黑 开水",
	"黑 坎子",
	"黑 2710",
	"黑 连子",
	"黑 半边字",
	"黑 对子",
};

//能否吃牌，对牌等 card对应于1-20，shown_hushu是之前的胡数
EatResult can_eat(int cards[21], int card, int shown_hushu)
{
	EatResult result;
	memset(&result, 0, sizeof(EatResult));
	int tmp[21] = { 0 };
	//先吃这个牌，然后判断是否胡牌
	memcpy(tmp, cards, sizeof(tmp));
	tmp[card]++;
	int hushu = get_hushu(tmp, card);
	if (hushu > -1 && shown_hushu + hushu > hupai_hushu) {
		result.can_hupai = 1;
		result.hushu = shown_hushu + hushu;
	}
	else {
		result.can_hupai = 0;
		result.hushu = -1;
	}
	//有开必开
	if (cards[card] == 3) {
		memcpy(tmp, cards, sizeof(tmp));
		tmp[card]++;

		result.eat_with[result.eat_count][0] = card;
		result.eat_with[result.eat_count][1] = card;
		result.eat_with[result.eat_count][2] = card;
		if (card <= 10)
			result.eat_with[result.eat_count][3] = EatTo_hei_4;//黑开水
		else
			result.eat_with[result.eat_count][3] = EatTo_hong_4;//红开水
		result.eat_count++;
		result.eat_kind = 5;//开
	}
	else {
		//去掉手里的4张，3张
		memcpy(tmp, cards, sizeof(tmp));
		for (int i = 1; i <= 20; i++)
		{
			if (tmp[i] == 4 || tmp[i] == 3)
				tmp[i] = 0;
		}

		//能否吃牌，能吃哪些牌，再判断是否需要摆牌和摆哪些牌
		if (card <= 10)
		{
			// x 7 10
			if (card == 2 && tmp[7] >= 1 && tmp[10] >= 1)
			{
				result.eat_with[result.eat_count][0] = 7;
				result.eat_with[result.eat_count][1] = 10;
				result.eat_with[result.eat_count][3] = EatTo_hei_2710;//黑2710
				result.eat_count++;
			}
			// 2 x 10
			if (card == 7 && tmp[2] >= 1 && tmp[10] >= 1)
			{
				result.eat_with[result.eat_count][0] = 2;
				result.eat_with[result.eat_count][1] = 10;
				result.eat_with[result.eat_count][3] = EatTo_hei_2710;//黑2710
				result.eat_count++;
			}
			//2 7 x
			if (card == 10 && tmp[2] >= 1 && tmp[7] >= 1)
			{
				result.eat_with[result.eat_count][0] = 2;
				result.eat_with[result.eat_count][1] = 7;
				result.eat_with[result.eat_count][3] = EatTo_hei_2710;//黑2710
				result.eat_count++;
			}
			// xxx
			if (tmp[card] == 2)
			{
				result.eat_with[result.eat_count][0] = card;
				result.eat_with[result.eat_count][1] = card;
				result.eat_with[result.eat_count][3] = EatTo_hei_3;//黑坎子
				result.eat_count++;
			}

			//三种顺子
			// 能组成：card-2,card-1,card
			if (card - 2 >= 1 && tmp[card - 2] >= 1 && tmp[card - 1] >= 1) {
				result.eat_with[result.eat_count][0] = card - 2;
				result.eat_with[result.eat_count][1] = card - 1;
				result.eat_with[result.eat_count][3] = EatTo_hei_lianzi;//黑连子
				result.eat_count++;
			}
			//能组成：card - 1, card, card + 1
			if ((card - 1 >= 1) && (card + 1 <= 10) && tmp[card - 1] >= 1 && tmp[card + 1] >= 1) {
				result.eat_with[result.eat_count][0] = card - 1;
				result.eat_with[result.eat_count][1] = card + 1;
				result.eat_with[result.eat_count][3] = EatTo_hei_lianzi;//黑连子
				result.eat_count++;
			}
			//能组成：card , card+1, card + 2
			if ((card + 2 <= 10) && tmp[card + 1] >= 1 && tmp[card + 2] >= 1) {
				result.eat_with[result.eat_count][0] = card + 1;
				result.eat_with[result.eat_count][1] = card + 2;
				result.eat_with[result.eat_count][3] = EatTo_hei_lianzi;//黑连子
				result.eat_count++;
			}
			//半边字 红红黑
			if (tmp[card + 10] >= 2)
			{
				result.eat_with[result.eat_count][0] = card + 10;
				result.eat_with[result.eat_count][1] = card + 10;
				result.eat_with[result.eat_count][3] = EatTo_hong_banbianzi;//红半边字
				result.eat_count++;
			}
			//半边字 黑黑红
			if (tmp[card] >= 2 && tmp[card + 10] >= 1)
			{
				result.eat_with[result.eat_count][0] = card;
				result.eat_with[result.eat_count][1] = card + 10;
				result.eat_with[result.eat_count][3] = EatTo_hei_banbianzi;//黑半边字
				result.eat_count++;
			}
		}
		//红色
		if (card > 10)
		{
			// x 7 10
			if (card == 12 && tmp[17] >= 1 && tmp[20] >= 1)
			{
				result.eat_with[result.eat_count][0] = 17;
				result.eat_with[result.eat_count][1] = 20;
				result.eat_with[result.eat_count][3] = EatTo_hong_2710;//红2710
				result.eat_count++;
			}
			// 2 x 10
			if (card == 17 && tmp[12] >= 1 && tmp[20] >= 1)
			{
				result.eat_with[result.eat_count][0] = 12;
				result.eat_with[result.eat_count][1] = 20;
				result.eat_with[result.eat_count][3] = EatTo_hong_2710;//红2710
				result.eat_count++;
			}
			//2 7 x
			if (card == 10 && tmp[12] >= 1 && tmp[17] >= 1)
			{
				result.eat_with[result.eat_count][0] = 12;
				result.eat_with[result.eat_count][1] = 17;
				result.eat_with[result.eat_count][3] = EatTo_hong_2710;//红2710
				result.eat_count++;
			}
			// xxx
			if (tmp[card] == 2)
			{
				result.eat_with[result.eat_count][0] = card;
				result.eat_with[result.eat_count][1] = card;
				result.eat_with[result.eat_count][3] = EatTo_hong_3;//红坎子
				result.eat_count++;
			}

			//三种顺子
			// 能组成：card-2,card-1,card
			if (card - 2 >= 11 && tmp[card - 2] >= 1 && tmp[card - 1] >= 1) {
				result.eat_with[result.eat_count][0] = card - 2;
				result.eat_with[result.eat_count][1] = card - 1;
				result.eat_with[result.eat_count][3] = EatTo_hong_lianzi;//红连子
				result.eat_count++;
			}
			//能组成：card - 1, card, card + 1
			if ((card - 1 >= 11) && (card + 1 <= 20) && tmp[card - 1] >= 1 && tmp[card + 1] >= 1) {
				result.eat_with[result.eat_count][0] = card - 1;
				result.eat_with[result.eat_count][1] = card + 1;
				result.eat_with[result.eat_count][3] = EatTo_hong_lianzi;//红连子
				result.eat_count++;
			}
			//能组成：card , card+1, card + 2
			if ((card + 2 <= 20) && tmp[card + 1] >= 1 && tmp[card + 2] >= 1) {
				result.eat_with[result.eat_count][0] = card + 1;
				result.eat_with[result.eat_count][1] = card + 2;
				result.eat_with[result.eat_count][3] = EatTo_hong_lianzi;//红连子
				result.eat_count++;
			}

			//半边字 黑黑红
			if (tmp[card - 10] >= 2)
			{
				result.eat_with[result.eat_count][0] = card - 10;
				result.eat_with[result.eat_count][1] = card - 10;
				result.eat_with[result.eat_count][3] = EatTo_hei_banbianzi;//黑半边字
				result.eat_count++;
			}
			//半边字 红红黑
			if (tmp[card] >= 2 && tmp[card - 10] >= 1)
			{
				result.eat_with[result.eat_count][0] = card - 10;
				result.eat_with[result.eat_count][1] = card;
				result.eat_with[result.eat_count][3] = EatTo_hong_banbianzi;//红半边字
				result.eat_count++;
			}
		}
		if (tmp[card] == 0 && result.eat_count == 0)//之前没有牌，并且也没有可以成房的，根本不能吃
			result.eat_kind = -1;
		else if (tmp[card] == 0 && result.eat_count >= 1)//之前没有牌，并且有1个或多个成房的，那么有1个或多个牌可以选择吃,不需要摆牌。
			result.eat_kind = 1;
		else if (tmp[card] == 1 && result.eat_count == 0)//之前有一张牌,并且也没有可以成房的，根本不能吃。
			result.eat_kind = -3;
		else if (tmp[card] == 1 && result.eat_count == 1)//之前有一张牌,只有一组可以成房。那么吃牌后摆不起。
			result.eat_kind = -2;
		else if (tmp[card] == 1 && result.eat_count > 1)//之前有一张牌，有多组可以成房。那么有1个或多个牌可以选择吃。
			result.eat_kind = 2;
		else if (tmp[card] == 2 && result.eat_count == 1)//之前有两张牌，并且只有一组可以成房（说明就只能组成3张的坎子），那么只有一组可以吃，组成坎子
			result.eat_kind = 3;
		else if (tmp[card] == 2 && result.eat_count > 1)//之前有两张牌，并且有多组可以成房（说明可以组成3张的坎子 + 连子、半边字、2710），有多组可以选则吃。
			result.eat_kind = 4;
	}
	return result;
}


void get_random_cards(int num, int *cards)
{
	int all_cards[80] = {};
	for (int i = 0; i < 80; i++)
	{
		all_cards[i] = i;
	}
	int left_count = 80;
	for (int i = 1; i <= num; i++)
	{

		int r = rand() % left_count;
		cards[i] = r + 1;//r是0-79，对应牌应该是1-80
		all_cards[i] = -1;
		left_count--;
		for (int j = i; j < left_count; j++)
		{
			all_cards[j] = all_cards[j + 1];
		}
	}
}

void collect_cards(int num, int *cards, int *collect_to)
{

	int collected[21] = { 0 };
	for (int i = 0; i < num; i++)
	{
		int card = cards[i];
		int color = (card - 1) / 40;
		int point = (card - 1) % 10 + 1;

		int index_of_20 = color * 10 + point;
		collected[index_of_20]++;
	}
	memcpy(collect_to, collected, sizeof(collected));
}

void dump_cards(int cards[21])
{
	for (int i = 1; i <= 20; i++)
	{
		int card_count = cards[i];
		if (card_count)
		{
			char * color = "黑";
			if (i <= 10)
				color = "黑";
			else
				color = "红";
			int point = i > 10 ? i - 10 : i;
			if (card_count == 1)
				printf("%s%d	", color, point);
			if (card_count == 2)
				printf("%s%d %d	", color, point, point);
			if (card_count == 3)
				printf("%s%d %d %d	", color, point, point, point);
			if (card_count == 4)
				printf("%s%d %d %d %d	", color, point, point, point, point);

			if (i == 10)
				printf("\n");
		}
	}
}
int main()
{
	if (freopen("file.txt", "w", stdout) == NULL)
		fprintf(stderr, "error\n");
	//    1  2  3  4  5  6  7  8  9  10
	int cards[21] = { 0,1,2,1,0,0,1,1,0,2,1,0,0,1,1,1,2,1,1,2,0 };
	EatResult result = can_eat(cards, 0, 0);
	int hushu = __get_hushu_no_gun_kan(cards);
	for (int i = 0; i < 1000; ++i)
	{
		if (i == 352)
			printf("");
		int test_cards[22] = {};
		get_random_cards(20, test_cards);
		int collected[21] = { 0 };
		collect_cards(21, test_cards, collected);
		printf("\n\n第%d次模拟判断开牌吃牌==============\n", i);
		dump_cards(collected);
		for (int card = 1; card <= 20; card++)
		{
			printf("\n当前尝试吃牌:");
			if (card == 17)
				printf("");
			int point = (card - 1) % 10 + 1;
			printf(card <= 10 ? "黑%d\n" : "红%d\n", point);
			EatResult result = can_eat(collected, card, 0);

			if (result.can_hupai)
				printf("是否可以胡牌：可以胡牌\n");
			else
				printf("是否可以胡牌：不能胡牌\n");

			if (result.eat_kind == -1)//不能吃
			{
				printf("根本吃不起\n");
			}
			else if (result.eat_kind == -2)//只能吃一个,还需要摆牌，所以不能摆牌，不能吃
			{
				printf("吃后摆不起，不能吃\n");
			}
			else if (result.eat_kind == 1)//之前没有牌，并且有1个或多个成房的，那么有1个或多个牌可以选择吃,不需要摆牌。
			{
				printf("直接可以吃（有一组或多组牌）,不需要摆牌。和");
				for (int k = 0; k < 3; k++) {
					int c = result.eat_with[0][k];
					if (c == 0)
						break;
					int point = (c - 1) % 10 + 1;
					printf(c <= 10 ? "黑%d " : "红%d ", point);
				}
				printf("组成");
				printf(EatName[result.eat_with[0][3]]);
				printf("\n");
			}
			else if (result.eat_kind == 2)//之前有一张牌，有多组可以成房。那么有多个牌可以选择吃,需要摆牌。
			{
				printf("直接吃，但是需要摆牌，和");
				for (int j = 0; j < result.eat_count; j++)
				{
					for (int k = 0; k < 3; k++) {
						int c = result.eat_with[j][k];
						if (c == 0)
							break;
						int point = (c - 1) % 10 + 1;
						printf(c <= 10 ? "黑%d " : "红%d ", point);
					}
					printf("组成");
					printf(EatName[result.eat_with[j][3]]);
					printf("\n");
				}
			}
			else if (result.eat_kind == -3)//之前有一张牌,并且也没有可以成房的，根本不能吃。
			{
				printf("之前有一张牌，不能组成房，不能吃");
			}
			else if (result.eat_kind == 3)//之前有两张牌，并且只有一组可以成房（说明就只能组成3张的坎子），那么只有一组可以吃，组成坎子，不需要摆牌
			{
				printf("吃组成坎子，不需要摆牌，和");
				for (int j = 0; j < result.eat_count; j++)
				{
					for (int k = 0; k < 3; k++) {
						int c = result.eat_with[j][k];
						if (c == 0)
							break;
						int point = (c - 1) % 10 + 1;
						printf(c <= 10 ? "黑%d " : "红%d ", point);
					}
					printf("组成");
					printf(EatName[result.eat_with[j][3]]);
					printf("\n");
				}
			}
			else if (result.eat_kind == 4)//之前有两张牌，并且有多组可以成房（说明可以组成3张的坎子 + 连子、半边字、2710），有多组可以选则吃。
			{
				printf("直接吃，但是需要摆牌，和");
				for (int j = 0; j < result.eat_count; j++)
				{
					for (int k = 0; k < 3; k++) {
						int c = result.eat_with[j][k];
						if (c == 0)
							break;
						int point = (c - 1) % 10 + 1;
						printf(c <= 10 ? "黑%d " : "红%d ", point);
					}
					printf("组成");
					printf(EatName[result.eat_with[j][3]]);
					printf("\n");
				}
			}
			else if (result.eat_kind == 5)//开
			{
				printf("开水，和");
				for (int j = 0; j < result.eat_count; j++)
				{
					for (int k = 0; k < 3; k++) {
						int c = result.eat_with[j][k];
						if (c == 0)
							break;
						int point = (c - 1) % 10 + 1;
						printf(c <= 10 ? "黑%d " : "红%d ", point);
					}
					printf("组成");
					printf(EatName[result.eat_with[j][3]]);
					printf("\n");
				}
			}

			if (result.can_hupai)
				printf("========可胡牌=====");
		}


		int hushu = get_hushu(collected);
		if (hushu > 0)//开牌可胡
		{
			printf("##########开牌就胡了，胡数:%d\n\n", hushu);
		}

	}

	return 0;
}