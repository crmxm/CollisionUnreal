// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"

inline float clamp(float in, float lo, float hi) { return (in < lo) ? lo : ((in > hi) ? hi : in); };
inline float Sqr(float a) { return a * a; };
