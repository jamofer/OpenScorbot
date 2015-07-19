/* PARAMS CONFIGURATION FOR SCORBOT ER IX
 * Target: BeagleBone/Debian
 * 
 * Version: 1.0
 * Autor: Fco. Javier Moragon Fernandez
 * Date: 2015/08/13						*/

#ifndef RobotPinConfig_H
#define RobotPinConfig_H

/* define varname to string trick */
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

/*  BASIC  */
	#define HROBOT	 	1
	#define LROBOT	 	0
	#define NUM_AXES 	6
	#define MAX_PWR 	0xF3
	#define BAD_VALUE 	0x7FFFFFFF
	
/*	AXIS NUMBER IDENTIFICATOR  */
	#define BASE			0
	#define BODY			1
	#define UPPER_ARM		2
	#define FOREARM_PITCH	3
	#define FOREARM_ROLL	4
	#define CLIP			5

/*  AXIS LIMITS  */
	#define BASE_COUNTS_MAX 			155000
	#define BASE_COUNTS_HOME			155000

	#define BODY_COUNTS_MAX				105000
	#define BODY_COUNTS_HOME			105000

	#define UPPER_ARM_COUNTS_MAX		99000
	#define UPPER_ARM_COUNTS_HOME		99000

	#define FOREARM_PITCH_COUNTS_MAX	99000
	#define FOREARM_PITCH_COUNTS_HOME	0

	#define FOREARM_ROLL_COUNTS_MAX		80000
	#define FOREARM_ROLL_COUNTS_HOME	0

	#define CLIP_COUNTS_MAX				12000
	#define CLIP_COUNTS_HOME			0

/*  AXIS PID VALUES  */
	#define BASE_PID_P				0.02f
	#define BASE_PID_I				0.000006f
	#define BASE_PID_D				0.01f

	#define BODY_PID_P				0.0088f
	#define BODY_PID_I				0.000004f
	#define BODY_PID_D				0.18f

	#define UPPER_ARM_PID_P			0.0085f
	#define UPPER_ARM_PID_I			0.0000035f
	#define UPPER_ARM_PID_D			0.01f

	#define FOREARM_PITCH_PID_P		0.03f
	#define FOREARM_PITCH_PID_I		0.000007f
	#define FOREARM_PITCH_PID_D		0.05f

	#define FOREARM_ROLL_PID_P		0.055f
	#define FOREARM_ROLL_PID_I		0.00006f
	#define FOREARM_ROLL_PID_D		0.075f

	#define CLIP_PID_P				0.02f
	#define CLIP_PID_I				0.001f
	#define CLIP_PID_D				0.000006f

/* HROBOT */
/* I2C */
	#define LR_ADDR					0x58 //In beaglebone, address is not shifted 1bit to the left like MBED
	#define I2C_DEVICE				"/dev/i2c-1"

/* LOGFILES */
	#define HROBOT_LOG "HRobot.log"
	#define LROBOT_LOG	"LRobot.log"
	
	
/*  LROBOT */
/*  ENCODERS, HOMESWITCHES PINS */

	//BASE
	#define ENC_BASE_CH_A			p5
	#define ENC_BASE_CH_B			p6
	#define HSW_BASE				p21
	
	//BODY
	#define ENC_BODY_CH_A			p7
	#define ENC_BODY_CH_B			p8
	#define HSW_BODY				p22
	
	//UPPER_ARM
	#define ENC_UPPER_ARM_CH_A		p11
	#define ENC_UPPER_ARM_CH_B		p12
	#define HSW_UPPER_ARM			p23
	
	//FOREARM_PITCH
	#define ENC_FOREARM_PITCH_CH_A	p13
	#define ENC_FOREARM_PITCH_CH_B	p14
	#define HSW_FOREARM_PITCH		p24
	
	//FOREARM_ROLL
	#define ENC_FOREARM_ROLL_CH_A	p15
	#define ENC_FOREARM_ROLL_CH_B	p16
	#define HSW_FOREARM_ROLL		p25
	
	//CLIP
	#define ENC_CLIP_CH_A			p17
	#define ENC_CLIP_CH_B			p18
	#define HSW_CLIP				p26
	

/* I2C PINS AND ADDRESES */

	//MotorControllerI2C
	#define MC_DATA					p28
	#define MC_CLK					p27
	
	#define MC_BASE_ADDR			0xB0
	#define MC_BODY_ADDR			0xB2
	#define MC_UPPER_ARM_ADDR		0xB4
	#define MC_FOREARM_PITCH_ADDR	0xB6
	#define MC_FOREARM_ROLL_ADDR	0xB8
	#define MC_CLIP_ADDR			0xBA
	
	//I2CSlave socket with HRobot
	#define HR_DATA					p9
	#define HR_CLK					p10
	
	#define HR_ADDR					0xB0 //in MBED I2C address is 1bit shifted to the left
#endif
	
	
		
