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

#include "Identifiers.h"

namespace IDs
{
#define DEFINE_ID(name) const Identifier name (#name);

    DEFINE_ID (StateRoot)

    DEFINE_ID (AudioFileName)
    DEFINE_ID (AudioFileLoaded)
    DEFINE_ID (AudioFileSampleRate)
    DEFINE_ID (AudioFileLengthSeconds)
    DEFINE_ID (AudioFileLeftViewSecond)
    DEFINE_ID (AudioFileRightViewSecond)

#undef DEFINE_ID
}
