#include "eyes.h"
#include "video.h"
#include <f1.h>
#include <rcc.h>
#include <cmsis.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static uint8_t CURRENT_BLINK_PHASE = 0;
static bool BLINKING = false;

static EYE_t* DEFAULT_LEFT_EYE  = eye_neutral;
static EYE_t* DEFAULT_RIGHT_EYE = eye_neutral;

static EYE_t* CURRENT_LEFT_EYE;
static EYE_t* CURRENT_RIGHT_EYE;

void eyes_init(void)
{
    video_init();
    start_blinking();
    select_eyes(DEFAULT_LEFT_EYE, DEFAULT_RIGHT_EYE);
}

void select_eyes(EYE_t eye_left, EYE_t eye_right)
{
    void *fb_ptr = &FRAME_BUFFER_SECONDARY;

    CURRENT_LEFT_EYE  = eye_left;
    CURRENT_RIGHT_EYE = eye_right;

    for (int i = 0; i < 8; i++)
    {
        memset(fb_ptr++, vec2byte(eye_right[i]), 1);
        memset(fb_ptr++, vec2byte(eye_left[i]), 1);
    }

    start_blink_animation();
}

void blink_advance_phase(void)
{
    switch (CURRENT_BLINK_PHASE)
    {
        case 0:
            FRAME_BUFFER[0] = 0x0000;
            FRAME_BUFFER[7] = 0x0000;
            CURRENT_BLINK_PHASE++;
            break;

        case 1:
            FRAME_BUFFER[1] = 0x0000;
            FRAME_BUFFER[6] = 0x0000;
            CURRENT_BLINK_PHASE++;
            break;

        case 2:
            FRAME_BUFFER[2] = 0x0000;
            FRAME_BUFFER[5] = 0x0000;
            CURRENT_BLINK_PHASE++;
            break;

        case 3:
            FRAME_BUFFER[3] = 0x0000;
            FRAME_BUFFER[4] = 0x0000;
            CURRENT_BLINK_PHASE++;
            break;

        case 4:
            FRAME_BUFFER[3] = 0x0000;
            CURRENT_BLINK_PHASE++;
            break;

        case 5:
            FRAME_BUFFER[4] = FRAME_BUFFER_SECONDARY[4];
            CURRENT_BLINK_PHASE++;
            break;

        case 6:
            FRAME_BUFFER[3] = FRAME_BUFFER_SECONDARY[3];
            CURRENT_BLINK_PHASE++;
            break;

        case 7:
            FRAME_BUFFER[2] = FRAME_BUFFER_SECONDARY[2];
            FRAME_BUFFER[5] = FRAME_BUFFER_SECONDARY[5];
            CURRENT_BLINK_PHASE++;
            break;

        case 8:
            FRAME_BUFFER[1] = FRAME_BUFFER_SECONDARY[1];
            FRAME_BUFFER[6] = FRAME_BUFFER_SECONDARY[6];
            CURRENT_BLINK_PHASE++;
            break;

        case 9:
            FRAME_BUFFER[0] = FRAME_BUFFER_SECONDARY[0];
            FRAME_BUFFER[7] = FRAME_BUFFER_SECONDARY[7];
            start_blinking();
            break;
    }
}

void start_blink_animation(void)
{
    TIM3_CR1->CEN = false;
    CURRENT_BLINK_PHASE = 0;
    BLINKING = true;

    // Setup timer
    __enable_irq();
    NVIC_EnableIRQ(TIM3_IRQ);
    NVIC_SetPriority(TIM3_IRQ, 1);
    RCC_APB1ENR->TIM3EN = true;
    TIM3_CR1->DIR = false; // Upcounter
    *TIM3_CNT = 0;
    *TIM3_PSC = 7200; // 10Khz
    *TIM3_ARR = 200; // 50Hz
    TIM3_DIER->UIE = true; // TIM3 interrupt enable
    TIM3_CR1->CEN = true; // Enable counter
}

void start_blinking(void)
{
    TIM3_CR1->CEN = false;
    BLINKING = false;
    CURRENT_BLINK_PHASE = 0;

    int rand_interval = (rand() % (10 + 1 - 2)) + 2;

    // Setup timer
    __enable_irq();
    NVIC_EnableIRQ(TIM3_IRQ);
    NVIC_SetPriority(TIM3_IRQ, 1);
    RCC_APB1ENR->TIM3EN = true;
    TIM3_CR1->DIR = false; // Upcounter
    *TIM3_CNT = 0;
    *TIM3_PSC = 60000; // 1.2Khz
    *TIM3_ARR = rand_interval * 6000;
    TIM3_DIER->UIE = true; // TIM3 interrupt enable
    TIM3_CR1->CEN = true; // Enable counter
}

void TIM3_ISR(void)
{
    if (BLINKING)
    {
        blink_advance_phase();
    }
    else
    {
        start_blink_animation();
    }

    TIM3_SR->UIF = false;
}

const EYE_t eye_neutral = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,1,1,1,1,0,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,1,1,1,1,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };

const EYE_t eye_surprised = {
      { 0,0,1,1,1,1,0,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,1,1,1,1,0,0 }
    };

const EYE_t eye_happy = {
      { 0,0,1,1,1,1,0,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };


const EYE_t eye_smile = {
      { 0,0,1,1,1,1,0,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 1,1,1,0,0,1,1,1 }
    , { 1,1,0,0,0,0,1,1 }
    , { 1,0,0,0,0,0,0,1 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };

const EYE_t eye_blink = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };

const EYE_t eye_squint = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };


const EYE_t eye_sleepy = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,1,1,1,1,0,0 }
    };


const EYE_t eye_angry_left = {
      { 0,0,0,0,0,0,0,0 }
    , { 1,1,1,1,0,0,0,0 }
    , { 1,1,1,1,1,1,0,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,1,1,1,1,0,0 }
    };


const EYE_t eye_angry_right = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,1,1,1,1 }
    , { 0,0,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,0 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,1,1,1,1,0,0 }
    };


const EYE_t eye_confused_left = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,0 }
    , { 0,1,1,1,1,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };


const EYE_t eye_confused_right = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 1,1,1,1,1,1,0,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,0,1,1,1,1,1,1 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };

const EYE_t eye_sad_up_left = {
      { 0,0,0,0,1,1,1,0 }
    , { 0,0,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,0 }
    , { 0,1,1,1,1,1,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };

const EYE_t eye_sad_up_right = {
      { 0,1,1,1,0,0,0,0 }
    , { 1,1,1,1,1,1,0,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,1 }
    , { 0,0,1,1,1,1,1,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };

const EYE_t eye_sad_down_left = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,1,1,1 }
    , { 0,0,0,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,0 }
    , { 1,1,1,1,1,1,0,0 }
    };

const EYE_t eye_sad_down_right = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 1,1,1,0,0,0,0,0 }
    , { 1,1,1,1,1,0,0,0 }
    , { 0,1,1,1,1,1,1,1 }
    , { 0,0,1,1,1,1,1,1 }
    };

const EYE_t eye_bored = {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 0,1,1,1,1,1,1,0 }
    , { 0,0,1,1,1,1,0,0 }
    };


const EYE_t eye_full= {
      { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    , { 1,1,1,1,1,1,1,1 }
    };


const EYE_t eye_empty= {
      { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    , { 0,0,0,0,0,0,0,0 }
    };


const EYE_t eye_dead = {
      { 1,0,0,0,0,0,0,1 }
    , { 0,1,0,0,0,0,1,0 }
    , { 0,0,1,0,0,1,0,0 }
    , { 0,0,0,1,1,0,0,0 }
    , { 0,0,0,1,1,0,0,0 }
    , { 0,0,1,0,0,1,0,0 }
    , { 0,1,0,0,0,0,1,0 }
    , { 1,0,0,0,0,0,0,1 }
    };
