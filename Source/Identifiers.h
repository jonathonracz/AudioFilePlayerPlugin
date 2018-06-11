/*
    Copyright (C) 2018 Jonathon Racz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "JuceHeader.h"

namespace IDs
{
#define DECLARE_ID(name) extern const Identifier name;

    DECLARE_ID (StateRoot)

    DECLARE_ID (AudioFileName)
    DECLARE_ID (AudioFileLoaded)
    DECLARE_ID (AudioFileLengthSeconds)
    DECLARE_ID (AudioFileLeftViewSecond)
    DECLARE_ID (AudioFileRightViewSecond)

#undef DECLARE_ID
}
