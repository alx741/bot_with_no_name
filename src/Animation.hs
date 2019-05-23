{-# LANGUAGE OverloadedStrings #-}
{-# OPTIONS_GHC -Wno-unused-do-bind #-}
{-# OPTIONS_GHC -Wno-missing-signatures #-}

module Animation where

import Control.Concurrent (threadDelay)
import Driver
import Voice              (Noise (..), makeNoise, say)

-- | Wait n milliseconds
wait :: Int -> IO ()
wait = threadDelay . (* 1000)

noiseDir = "./data/sound"
makeNoise' = makeNoise noiseDir

wakeUpPhase1 :: IO ()
wakeUpPhase1 = do
    setEyes Suspicious
    wait 3000
    makeNoise' WakeupMumbleUp
    setHead (position 5) (position 5)
    wait 1000
    setEyes Bored
    wait 3000
    setEyes Suspicious
    wait 2000
    makeNoise' WakeupMumbleDown
    setHead (position 0) (position 5)
    wait 1000
    robot Shutdown

wakeUpPhase2 :: IO ()
wakeUpPhase2 = do
    setEyes Suspicious
    wait 3000
    makeNoise' WakeupMumbleUp
    setHead (position 5) (position 5)
    wait 3000
    setEyes Angry
    setHead (position 5) (position 0)
    makeNoise' Groan
    wait 2000
    makeNoise' Groan
    setHead (position 5) (position 10)
    wait 2000
    setHead (position 5) (position 5)
    wait 2000
    setEyes Bored
    wait 2000
    setEyes Neutral

sleep :: IO ()
sleep = do
    setEmotion Neutral
    wait 2000
    setEyes Bored
    wait 2000
    setEyes Suspicious
    wait 2000
    makeNoise' WakeupMumbleDown
    setHead (position 0) (position 5)
    wait 2000
    robot Shutdown

sayName :: IO ()
sayName = do
    setEmotion Neutral
    wait 1000
    setEmotion Smiley
    say "me llamo dors"
    wait 1500
    makeNoise' LittleLaughter
    wait 3000
    setEmotion Neutral
