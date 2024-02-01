#pragma once

#include <iostream>

#include "Window.hpp"
#include "Instance.hpp"

namespace re {

	class RathalosEngine {
	public:
		RathalosEngine(void);
		~RathalosEngine(void);
	private:
		re::Instance instance;
	};

}
