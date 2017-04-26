#pragma once

namespace graphics {
	class Graphics
	{
	  public:
	  	Graphics(int const windowWidth, int const windowHeight);
	    void init();
      private:
      	void display();
      	void reshape();
      	void keyboardHandler(int const key, int const x, int const y);
      	void passiveMouseFunc(int const x, int const y);
	};
}