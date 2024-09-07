# MMP-H264

English | [简体中文](./README.md)

## Overview

Lightweight H264 syntax parser.

## Features

- Written in C11, adhering to RAII standards for simple memory management
- Consistent with the syntax parsing style of ISO 14496-10 (2020-12) for easy reference
- Written using STL, can be independently built and compiled for easy integration

## How to Use

Implement `AbstractH264ByteReader`. For specific usage, refer to `main.cpp`.

## About Logging

Generally, SDK logging management can be categorized into different methods, such as:

- Internal maintenance, where the SDK caller specifies the log storage location
- External export, providing methods like log callbacks

Or both methods can coexist.

However, `MMP-H26X` is a very lightweight H264/H265 syntax parsing library, typically included directly as source code rather than as a dynamic or static library. Therefore, the logging export method of `MMP-H26X` is implemented through predefined macro definition anchors. For specific examples, refer to: `https://github.com/HR1025/MMP-Core/pull/5`

> Additionally, `MMP-CORE` also serves as a code reference example for `MMP-H26X`.

## TODO

- Implement H264 syntax encapsulation
- Support H265 (HEVC) syntax parsing encapsulation# MMP-H264