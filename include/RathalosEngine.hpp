#pragma once

#include <iostream>

#include "Window.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "Device.hpp"

namespace re {

	class RathalosEngine {
	public:
		RathalosEngine(void);
		~RathalosEngine(void);
		
		re::Window window{ "Rathalos Engine", 1280, 720 };
		re::Instance instance;
		re::Surface surface{ window, instance };
		re::Device device{ instance, surface };

	private:

	};

}
