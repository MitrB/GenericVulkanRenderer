CFLAGS = -std=c++17 -O2 -I.
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

Vulkan: *.cpp *.hpp
	g++ $(CFLAGS) -o Vulkan *.cpp $(LDFLAGS)

.PHONY: all test clean

test: Vulkan
	./Vulkan

clean:
	rm -f Vulkan


