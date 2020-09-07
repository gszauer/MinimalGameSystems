#ifndef _H_APPLICATION_
#define _H_APPLICATION_


class Application {
private:
	
private:
	Application(const Application&);
	Application& operator=(const Application&);
public:
	Application();
	~Application();

	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif
