#include "Motor.h"
 
char id_motor = 0;  //电机ID

motor_set motor =   //电机结变量
{
    .id = {0},
    .pulse_num = 0,
    .Subdivision = matlab_Subdivision,
    .step_max = 200*matlab_Subdivision,
    .sign_1_ok_step = 0,
};

//分频值：2 ,细分系数: 8  
//最大角速度为377.627452  度/S
//最小角速度为132.878205  度/S
//总阶段运行总时长为1.250195 s
const unsigned int sj_TIM_ARR[400] = {
    60958,    60726,    60496,    60266,    60038,    59811,    59585,    59360,    59136,    58913,
    58691,    58470,    58251,    58032,    57815,    57599,    57383,    57169,    56956,    56744,
    56533,    56323,    56114,    55906,    55699,    55493,    55288,    55084,    54881,    54679,
    54478,    54279,    54080,    53882,    53685,    53489,    53294,    53100,    52907,    52715,
    52524,    52333,    52144,    51956,    51769,    51582,    51397,    51212,    51029,    50846,
    50664,    50483,    50303,    50124,    49946,    49769,    49593,    49417,    49242,    49069,
    48896,    48724,    48553,    48382,    48213,    48045,    47877,    47710,    47544,    47379,
    47214,    47051,    46888,    46726,    46565,    46405,    46246,    46087,    45929,    45772,
    45616,    45461,    45306,    45152,    44999,    44847,    44696,    44545,    44395,    44246,
    44097,    43950,    43803,    43657,    43511,    43367,    43223,    43079,    42937,    42795,
    42654,    42514,    42374,    42235,    42097,    41960,    41823,    41687,    41552,    41417,
    41283,    41150,    41017,    40886,    40754,    40624,    40494,    40365,    40236,    40108,
    39981,    39855,    39729,    39603,    39479,    39355,    39232,    39109,    38987,    38865,
    38745,    38624,    38505,    38386,    38268,    38150,    38033,    37916,    37801,    37685,
    37571,    37457,    37343,    37230,    37118,    37006,    36895,    36784,    36674,    36565,
    36456,    36348,    36240,    36133,    36026,    35920,    35815,    35710,    35606,    35502,
    35398,    35296,    35193,    35092,    34991,    34890,    34790,    34690,    34591,    34493,
    34395,    34297,    34200,    34103,    34008,    33912,    33817,    33723,    33629,    33535,
    33442,    33349,    33257,    33166,    33075,    32984,    32894,    32804,    32715,    32627,
    32538,    32451,    32363,    32276,    32190,    32104,    32018,    31933,    31849,    31765,
    31681,    31598,    31515,    31433,    31351,    31269,    31188,    31107,    31027,    30947,
    30868,    30789,    30710,    30632,    30554,    30477,    30400,    30324,    30248,    30172,
    30097,    30022,    29947,    29873,    29799,    29726,    29653,    29581,    29508,    29437,
    29365,    29294,    29224,    29153,    29083,    29014,    28945,    28876,    28808,    28740,
    28672,    28604,    28538,    28471,    28405,    28339,    28273,    28208,    28143,    28079,
    28014,    27950,    27887,    27824,    27761,    27699,    27636,    27575,    27513,    27452,
    27391,    27330,    27270,    27210,    27151,    27092,    27033,    26974,    26916,    26858,
    26800,    26743,    26686,    26629,    26572,    26516,    26460,    26405,    26350,    26295,
    26240,    26186,    26132,    26078,    26024,    25971,    25918,    25865,    25813,    25761,
    25709,    25658,    25606,    25555,    25505,    25454,    25404,    25354,    25305,    25255,
    25206,    25157,    25109,    25060,    25012,    24964,    24917,    24870,    24823,    24776,
    24729,    24683,    24637,    24591,    24546,    24500,    24455,    24410,    24366,    24322,
    24277,    24234,    24190,    24147,    24103,    24061,    24018,    23975,    23933,    23891,
    23849,    23808,    23767,    23725,    23685,    23644,    23603,    23563,    23523,    23483,
    23444,    23404,    23365,    23326,    23288,    23249,    23211,    23173,    23135,    23097,
    23059,    23022,    22985,    22948,    22911,    22875,    22839,    22803,    22767,    22731,
    22695,    22660,    22625,    22590,    22555,    22521,    22486,    22452,    22418,    22384,
    22350,    22317,    22284,    22251,    22218,    22185,    22152,    22120,    22088,    22056,
    22024,    21992,    21961,    21929,    21898,    21867,    21836,    21805,    21775,    21744,
    21714,    21684,    21654,    21625,    21595,    21566,    21536,    21507,    21478,    2000
};

 unsigned int z_TIM_ARR[88] = {
    58113,    56655,    55241,    53869,    52537,    51245,    49992,    48776,    47597,    46454,
    45345,    44270,    43228,    42218,    41238,    40289,    39369,    38478,    37614,    36777,
    35966,    35181,    34419,    33682,    32968,    32276,    31607,    30958,    30330,    29721,
    29132,    28562,    28009,    27475,    26957,    26456,    25971,    25501,    25047,    24607,
    24181,    23769,    23370,    22984,    22611,    22249,    21900,    21561,    21234,    20917,
    20610,    20313,    20026,    19749,    19480,    19220,    18969,    18725,    18490,    18262,
    18042,    17829,    17623,    17423,    17231,    17044,    16864,    16689,    16520,    16357,
    16199,    16047,    15899,    15756,    15618,    15484,    15355,    15230,    15109,    14992,
    14879,    14770,    14664,    14562,    14463,    14367,    14275,    14185
};
unsigned int length = sizeof(z_TIM_ARR) / sizeof(z_TIM_ARR[0]);
unsigned int sj_length = sizeof(sj_TIM_ARR) / sizeof(sj_TIM_ARR[0]);

/**************************************************************************
函数功能：电机初始化
入口参数：无
返回  值：无
**************************************************************************/
void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    NVIC_InitTypeDef  NVIC_Initstruct = {0};

    RCC_APB2PeriphClockCmd(MOTOR_OFFLINE_GPIO_CLK,ENABLE);      //使能引脚时钟
    RCC_APB2PeriphClockCmd(MOTOR_DIRECT_GPIO_CLK,ENABLE);       
    RCC_APB2PeriphClockCmd(MOTOR_PULSE_GPIO_CLK,ENABLE);        
    RCC_APB2PeriphClockCmd(MOTOR_TIMER_CLK,ENABLE);             //使能定时器时钟

    GPIO_InitStructure.GPIO_Pin = MOTOR_OFFLINE_GPIO_PIN_1|MOTOR_OFFLINE_GPIO_PIN_2|
                                  MOTOR_OFFLINE_GPIO_PIN_3|MOTOR_OFFLINE_GPIO_PIN_4; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_OFFLINE_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(MOTOR_OFFLINE_PORT,MOTOR_OFFLINE_GPIO_PIN_1|MOTOR_OFFLINE_GPIO_PIN_2|
                                      MOTOR_OFFLINE_GPIO_PIN_3|MOTOR_OFFLINE_GPIO_PIN_4);  

    GPIO_InitStructure.GPIO_Pin = MOTOR_DIRECT_GPIO_PIN_1|MOTOR_DIRECT_GPIO_PIN_2|
                                  MOTOR_DIRECT_GPIO_PIN_3|MOTOR_DIRECT_GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_DIRECT_PORT,&GPIO_InitStructure);
    GPIO_SetBits(MOTOR_DIRECT_PORT,MOTOR_DIRECT_GPIO_PIN_1|MOTOR_DIRECT_GPIO_PIN_2|
                                   MOTOR_DIRECT_GPIO_PIN_3|MOTOR_DIRECT_GPIO_PIN_4);
    
    GPIO_InitStructure.GPIO_Pin = MOTOR_PULSE_GPIO_PIN_1|MOTOR_PULSE_GPIO_PIN_2|
                                  MOTOR_PULSE_GPIO_PIN_3|MOTOR_PULSE_GPIO_PIN_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_PULSE_PORT,&GPIO_InitStructure);
    GPIO_ResetBits(MOTOR_PULSE_PORT,MOTOR_PULSE_GPIO_PIN_1|MOTOR_PULSE_GPIO_PIN_2|
                                    MOTOR_PULSE_GPIO_PIN_3|MOTOR_PULSE_GPIO_PIN_4);

    TIM_DeInit(MOTOR_TIMER);                                                        //复位定时器
    TIM_TimeBaseStructure.TIM_Period = MOTOR_TIMER_ARR;                             //设置自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = MOTOR_TIMER_PSC;                          //设置用来作为定时器时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                    //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                     //TIM 向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = MOTOR_SINGLE_PULSE - 1;           //输出的脉冲周期数 cnt + 1
    TIM_TimeBaseInit(MOTOR_TIMER,&TIM_TimeBaseStructure);                           //初始化 TIMx 的时间基数单位
            
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                               //选择定时器模式:PWM1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                   //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 1;                                              //设置待装入捕获比较寄存器的脉冲值
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                       //输出极性:TIM 输出比较极性高
            
    TIM_OC1Init(MOTOR_TIMER,&TIM_OCInitStructure);                                  //初始化外设定时器通道
    TIM_OC2Init(MOTOR_TIMER,&TIM_OCInitStructure);      
    TIM_OC3Init(MOTOR_TIMER,&TIM_OCInitStructure);      
    TIM_OC4Init(MOTOR_TIMER,&TIM_OCInitStructure);      
            
    TIM_CtrlPWMOutputs(MOTOR_TIMER,ENABLE);                                         //MOE 主输出使能
            
    TIM_OC1PreloadConfig(MOTOR_TIMER,TIM_OCPreload_Enable);                         //通道预装载使能
    TIM_OC2PreloadConfig(MOTOR_TIMER,TIM_OCPreload_Enable);     
    TIM_OC3PreloadConfig(MOTOR_TIMER,TIM_OCPreload_Enable);     
    TIM_OC4PreloadConfig(MOTOR_TIMER,TIM_OCPreload_Enable);     
    TIM_ARRPreloadConfig(MOTOR_TIMER,ENABLE);                                       //使能定时器在ARR上的预装载寄存器
            
    TIM_ClearITPendingBit(MOTOR_TIMER,TIM_IT_Update);                               //清除溢出中断标志位
    TIM_ITConfig(MOTOR_TIMER,TIM_IT_Update,ENABLE );                                //使能定时器更新中断

    NVIC_Initstruct.NVIC_IRQChannel = MOTOR_TIMER_INTERRUPT;                        //设置中断源
    NVIC_Initstruct.NVIC_IRQChannelPreemptionPriority = MOTOR_TIMER_IT_PRI_PRE;     //抢占优先级
    NVIC_Initstruct.NVIC_IRQChannelSubPriority = MOTOR_TIMER_IT_PRI_SUB;            //子优先级
    NVIC_Initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_Initstruct);
    
    TIM_Cmd(MOTOR_TIMER, DISABLE);                                                  //失能 TIM
}

/**************************************************************************
函数功能：电机及转动方向选择  
入口参数：Dir:选择正反转、ID 选择电机 
返回  值：无
**************************************************************************/
static void dir_control(bool dir,char id)
{
    switch(id)
    {
        case MOTOR_ID_1:
            { 
                MOTOR_TIMER_CHANNEL_4 = 0;
                MOTOR_TIMER_CHANNEL_3 = 0;
                MOTOR_TIMER_CHANNEL_2 = 0;
                MOTOR_TIMER_CHANNEL_1 = MOTOR_TIMER_ARR_REG / 2;
                if(dir == 1)
                {
                    MOTOR_DIRECT_OUT_1 = 1;
                }
                else
                {
                    MOTOR_DIRECT_OUT_1 = 0;
                }
            }break;
        case MOTOR_ID_2:
            {
                MOTOR_TIMER_CHANNEL_4 = 0;
                MOTOR_TIMER_CHANNEL_3 = 0;
                MOTOR_TIMER_CHANNEL_1 = 0;
                MOTOR_TIMER_CHANNEL_2 = MOTOR_TIMER_ARR_REG / 2;
                if(dir == 1)
                {
                    MOTOR_DIRECT_OUT_2 = 1;
                }
                else
                {
                    MOTOR_DIRECT_OUT_2 = 0;
                }
            }break;
        case MOTOR_ID_3:
            {
                MOTOR_TIMER_CHANNEL_4 = 0;
                MOTOR_TIMER_CHANNEL_2 = 0;
                MOTOR_TIMER_CHANNEL_1 = 0;
                MOTOR_TIMER_CHANNEL_3 = MOTOR_TIMER_ARR_REG / 2; 
                if(dir == 1)
                {
                    MOTOR_DIRECT_OUT_3 = 1; 
                }
                else
                {
                    MOTOR_DIRECT_OUT_3 = 0;
                }
            }break;
        case MOTOR_ID_4:
            {
                MOTOR_TIMER_CHANNEL_3 = 0;
                MOTOR_TIMER_CHANNEL_2 = 0;
                MOTOR_TIMER_CHANNEL_1 = 0;
                MOTOR_TIMER_CHANNEL_4 = MOTOR_TIMER_ARR_REG / 2;
                if(dir == 1)
                {
                    MOTOR_DIRECT_OUT_4 = 1;
                }
                else
                {
                    MOTOR_DIRECT_OUT_4 = 0;
                }
            }break;
        default:break;
    }
}

/**************************************************************************
函数功能：控制电机转动步数
入口参数：step 步数
返回  值：无
**************************************************************************/
static void step_control(u16 step)
{
    motor.step_max = step;
    TIM_Cmd(MOTOR_TIMER,ENABLE);    //使能 
    while(!motor.sign_1_ok_step);   //等待步数完成标志
    motor.sign_1_ok_step = 0;       //重置标志
}

/**************************************************************************
函数功能：控制电机转动圈数
入口参数：circle 圈数
返回  值：无
**************************************************************************/
static void circle_control(u32 circle)
{
    u32 count;
    motor.step_max = 200 * motor.Subdivision / MOTOR_SINGLE_PULSE;  //恢复一次一圈
    for(count = 0;count < circle;count++)
    {
        TIM_Cmd(MOTOR_TIMER, ENABLE);   //使能 TIM8
        while(!motor.sign_1_ok_step);   //等待一圈完成标志
        motor.sign_1_ok_step = 0;       //重置标志
    }
}

/**************************************************************************
函数功能：S型加减速(包括所有运动情况)
入口参数：无
返回  值：无
**************************************************************************/
static void MotorSpeedCtrl_S(unsigned int id,u32 pulse_num)
{
    long i;
    motor.step_max = 1;                                     // 单步走
    if(id == 1)
    {
        if(pulse_num > 2*length||pulse_num == 2*length)     // x >= 2n
        {
            for(i = 0;i < length;i++)                       /* 升 */
            {
                MOTOR_TIMER_ARR_REG = z_TIM_ARR[i];         // 装载计数值
                TIM_Cmd(MOTOR_TIMER,ENABLE);                // 使能定时器
                while(!motor.sign_1_ok_step);               // 等待步数完成
                motor.sign_1_ok_step = 0;                   // 重置标志
            }   
            for(i = 0;i < (pulse_num-2*length);i++)         /* 匀速 */
            {
                TIM_Cmd(MOTOR_TIMER,ENABLE);                // 使能定时器
                while(!motor.sign_1_ok_step);               // 等待步数完成
                motor.sign_1_ok_step = 0;                   // 重置标志
            }   
            for(i = length - 1;i >= 0 ;i--)                 /* 降 */
            {
                MOTOR_TIMER_ARR_REG = z_TIM_ARR[i];         // 装载计数值
                TIM_Cmd(MOTOR_TIMER,ENABLE);                // 使能定时器
                while(!motor.sign_1_ok_step);               // 等待步数完成
                motor.sign_1_ok_step = 0;                   // 重置标志
            }
        }
        else
        {                                                   // x < 2n
            for(i = 0;i < pulse_num/2;i++)                  /* 升 */ 
            {
                MOTOR_TIMER_ARR_REG = z_TIM_ARR[i];         // 装载计数值（整形除以2，取整，若x为奇数，3/2=1）
                TIM_Cmd(MOTOR_TIMER,ENABLE);                // 使能定时器
                while(!motor.sign_1_ok_step);               // 等待步数完成
                motor.sign_1_ok_step = 0;                   // 重置标志
            }  
            if(pulse_num%2 != 0)                            /* 匀速 */
            {                               
                TIM_Cmd(MOTOR_TIMER,ENABLE);                // 使能定时器（x为奇数步,则匀速一步）
                while(!motor.sign_1_ok_step);               // 等待步数完成
                motor.sign_1_ok_step = 0;                   // 重置标志
            }
            for(i = pulse_num/2 - 1;i >= 0;i--){            /* 降 */
                MOTOR_TIMER_ARR_REG = z_TIM_ARR[i];         // 装载计数值
                TIM_Cmd(MOTOR_TIMER,ENABLE);                // 使能定时器
                while(!motor.sign_1_ok_step);               // 等待步数完成
                motor.sign_1_ok_step = 0;                   // 重置标志
            }
        }
    }
    else if(id == 2)
    {
        for(i = 0;i < pulse_num - sj_length;i++)            /* 匀速 */
        {
            MOTOR_TIMER_ARR_REG = 2000 ;                    // 装载计数值
            TIM_Cmd(MOTOR_TIMER,ENABLE);                    // 使能定时器
            while(!motor.sign_1_ok_step);                   // 等待步数完成
            motor.sign_1_ok_step = 0;                       // 重置标志
        }       
        for(i = sj_length - 1;i >= 0 ;i--)                  /* 降速 */
        {
            MOTOR_TIMER_ARR_REG = sj_TIM_ARR[i];            // 装载计数值
            TIM_Cmd(MOTOR_TIMER,ENABLE);                    // 使能定时器
            while(!motor.sign_1_ok_step);                   // 等待步数完成
            motor.sign_1_ok_step = 0;                       // 重置标志
        }
    }
}
/**************************************************************************
函数功能：角度控制: (-∞ ~ +∞)
入口参数：TargetAngle:目标角度、ID: 电机码
返回  值：电机设置上电自动回中，角度顺增逆减 
**************************************************************************/
void  MotorAngleCtrl_S(float target_angle,char id)
{
    float angle_difference;                             // 角度差
    long pulse_num;
    id_motor = 0;
    switch(id){
        case MOTOR_ID_1:angle_difference = target_angle - motor.id[MOTOR_ID_1].angle;break;
        case MOTOR_ID_2:angle_difference = target_angle - motor.id[MOTOR_ID_2].angle;break;
        case MOTOR_ID_3:angle_difference = target_angle - motor.id[MOTOR_ID_3].angle;break;
        case MOTOR_ID_4:angle_difference = target_angle - motor.id[MOTOR_ID_4].angle;break; 
        default: break;
    }
    pulse_num = 200 * motor.Subdivision * angle_difference / 360.0;     // 得到脉冲差
    if(id == 1)                                         // 旋转
    {
        if(pulse_num >= 0)
        {
            MOTOR_DIRECT_OUT_1 = 1;                     // 正转
        }
        else                                            // 反转
        {
            pulse_num = -pulse_num; 
            MOTOR_DIRECT_OUT_1 = 0;
        }
        MOTOR_TIMER_PSC_REG = matlab_xz_pcr;            // 装载 旋转 分频值
        MOTOR_TIMER_CHANNEL_4 = 0;
        MOTOR_TIMER_CHANNEL_3 = 0;
        MOTOR_TIMER_CHANNEL_2 = 0;
        MOTOR_TIMER_CHANNEL_1 = z_TIM_ARR[length-1]/2;  // 选中1号电机,占空比尽量大，减少零电平时间过短检测不到变化沿的风险
        MotorSpeedCtrl_S(id,pulse_num);
    }
    else if(id == 2)                                    // 升降
    {
        if(pulse_num >= 0)                              // 上升
        { 
            MOTOR_DIRECT_OUT_2 = 1;  
            target_angle = motor.id[id].angle;  
        }                                   
        else                                            // 下降
        {
            pulse_num = -pulse_num; 
            MOTOR_DIRECT_OUT_2 = 0;
            MOTOR_TIMER_PSC_REG = matlab_sj_pcr;        // 装载 升降 分频值
            MOTOR_TIMER_CHANNEL_4 = 0;
            MOTOR_TIMER_CHANNEL_3 = 0;
            MOTOR_TIMER_CHANNEL_1 = 0;
            MOTOR_TIMER_CHANNEL_2 = 1000;
            MotorSpeedCtrl_S(id,pulse_num);
        }
    }
    motor.id[id].angle = target_angle;                  // 更新角度
}

/**************************************************************************
函数功能：不带加减速的角度控制( -∞ ~ +∞ )
入口参数：speed:无量纲转速（0~65535越大速度越大）、angle:目标角度、id:电机码
返回  值：电机设置上电自动回中,角度顺增逆减
**************************************************************************/
void MotorAngleCtrl(u16 speed,float target_angle,char id)
{
    float angle_difference;                 // 角度差
    long pulse_num;
    u32 r,step;                             // 整圈数、余步数
    id_motor = 1; 
    MOTOR_TIMER_PSC_REG = 72 ;              // 装载分频值
    MOTOR_TIMER_ARR_REG = 65535 - speed;    // 装载计数值
    switch(id)
    {
        case MOTOR_ID_1:angle_difference = target_angle - motor.id[MOTOR_ID_1].angle;break;
        case MOTOR_ID_2:angle_difference = target_angle - motor.id[MOTOR_ID_2].angle;break;
        case MOTOR_ID_3:angle_difference = target_angle - motor.id[MOTOR_ID_3].angle;break;
        case MOTOR_ID_4:angle_difference = target_angle - motor.id[MOTOR_ID_4].angle;break; 
        default:break;
    }
    pulse_num = motor.Subdivision * 200 * angle_difference / 360.0;     // 得到脉冲差
    if(pulse_num >= 0)                                                  // 正转
    {
        dir_control(1,id);
    }
    else                                                                // 反转
    {
        pulse_num = -pulse_num;
        dir_control(0,id);
    } 
    r = pulse_num / (motor.Subdivision * 200);                          // 整圈数
    step = pulse_num % (motor.Subdivision * 200);                       // 余步数
    circle_control(r);
    step_control(step);
    motor.id[id].angle = target_angle;                                  // 更新角度
}

/**************************************************************************
函数功能：定时器中断服务函数（电机脉冲控制）
入口参数：无
返回  值：无
**************************************************************************/
void MOTOR_TIMER_IT_FUNC(void)  
{ 
    static u32 step = 0;
    if(TIM_GetITStatus(MOTOR_TIMER_IT_FLAG,TIM_IT_Update) != RESET)    
    {
        if(id_motor == 1)
        {
            step++;  
            if(step >= motor.step_max)
            { 
                step = 0;
                motor.sign_1_ok_step = 1;           // 一步完成
                TIM_Cmd(MOTOR_TIMER, DISABLE);      // 失能定时器
            }
        }
        else
        {
            motor.sign_1_ok_step = 1;               // 一步完成
            TIM_Cmd(MOTOR_TIMER, DISABLE);          // 失能定时器
        }
        TIM_ClearITPendingBit(MOTOR_TIMER_IT_FLAG,TIM_IT_Update);   // 清除溢出中断标志位
    }
}

/**************************************************************************
函数功能：升降电机运动
入口参数：Pos：电机位置
返回  值：无
注意事项：电机进行任何运动之前尽量先复位！！！
--高度单位除以100后为cm！！！提高分辨率
--零点时升降电机为 0 度，然后测得抓夹降至地面时为 x 度
--零点时抓夹高度为 H mm，抓夹降至地面时为 h mm
--则可以算得电机旋转 1 度下降 (H - h)/ x mm
--即需要下降至距离地面 Height mm 时 需要 (H - Height)*(H - h)/x 度
--标准物料抓取高度为 550 mm
**************************************************************************/
u32 MotorMotion_Lift(Lifting Pos,bool Wait)   
{
    static u32 HeightNow = ZeroHeight;                     //机械臂实时高度
    if(Pos != Places && Pos != Stack)
    {
        switch(Pos)
        {
            case Reset_L:HeightNow = ZeroHeight;break;      //复位（升最高）
            case Grab:   HeightNow = 475; break;            //降标准物件抓取高度（距离地面 525 mm） 
            case RawArea:HeightNow = 1375;break;            //降标准物件抓取转盘高度
            case Stuff:  HeightNow = 1740;break;            //降标准物件放置托盘高度
            case Aim:    HeightNow = 575; break;            //降校准
            case Raw:    HeightNow = 1375;break;            //降原料区识别
            case Pile:   HeightNow = 875;break;             //降码垛校准 
            case Rise:
                {
                    HeightNow += 50;
                    if(HeightNow > ZeroHeight + 25)
                    {
                        HeightNow = ZeroHeight;             //升高50 mm
                    }
                }break;
            case Fall:
                {
                    HeightNow -= 50;
                    if(HeightNow < LowestHeight - 25)
                    {
                        HeightNow = LowestHeight;           //降低50 mm
                    }
                }break;
            case Lowest:HeightNow = LowestHeight;break;     //降最低(距离地面 325 mm)
        }
        if(HeightNow <= ZeroHeight && HeightNow >= LowestHeight)
        {
            MotorAngleCtrl(MOTOR_SPEED,-((ZeroHeight - HeightNow)/Division),2);
        }
    }
    else
    {
        switch(Pos)
        {
            case Places:HeightNow = 475; break;             //降标准物件抓取高度（距离地面 525 mm）
            case Stack: HeightNow = 1175;break;             //降标准物件码垛高度（距离地面 1225 mm）
        }
         MotorAngleCtrl_S(-((ZeroHeight - HeightNow)/Division),2);
    }
    if(Wait)
    {
        Delay_ms(150);
    }
    return HeightNow;
}

/**************************************************************************
函数功能：旋转电机运动
入口参数：Pos：电机位置
返回  值：无
注意事项：电机进行任何运动之前尽量先复位！！！
**************************************************************************/
void MotorMotion_Rotate(Rotate Pos,bool Wait)       
{
    switch(Pos)
    {
        case Reset_R: MotorAngleCtrl_S(0,1);break;          //复位
        case Vision:  MotorAngleCtrl_S(-164,1);break;       //旋转
    }
    if(Wait) 
    {
        Delay_ms(250);
    }
}
