#pragma once
#include "SuperShiny.h"

namespace ssge
{
	class Factory
	{
		Factory() = delete;
		Factory(const Factory& toCopy) = delete;
		Factory(Factory&& toMove) = delete;
		~Factory() = delete;
	public:
		class ForProgram
		{
			friend class Program;
			ForProgram() = delete;
			ForProgram(const Factory& toCopy) = delete;
			ForProgram(ForProgram&& toMove) = delete;
			~ForProgram() = delete;

		};
	};
}