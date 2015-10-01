-include Makefile.sources

CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++11 -Wconversion -g -Weverything \
	   -Wsign-conversion -Wno-c++98-compat -Wno-padded -Wno-documentation
CPPFLAGS = -I ${PWD}/src/
LDFLAGS =

TARGET=raytracer

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CXX)  $(LDFLAGS) $(OBJS) -o $(TARGET)

run:$(TARGET)
	./$(TARGET)

_build/%.o: src/%.cpp
	@ mkdir -p _build
	@ mkdir -p _build/light
	@ mkdir -p _build/camera
	@ mkdir -p _build/extra
	@ mkdir -p _build/obj_parser
	 $(CXX) -M $(CPPFLAGS) $(CXXFLAGS) $< -o _build/$*.d
	@ cat _build/$*.d | sed -e 's/[\\ ]/\n/g' | \
	    sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' > _build/$*.tags
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@mv -f _build/$*.d _build/$*.d.tmp
	@sed -e 's|.*:|_build/$*.o:|' < _build/$*.d.tmp > _build/$*.d
	@sed -e 's/.*://' -e 's/\\$$//' < _build/$*.d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> _build/$*.d
	@rm -f _build/$*.d.tmp

clean:
	$(RM) -r _build $(TARGET)

tag:_build
	cat _build/*/*.tags _build/*.tags | ctags -L - --c++-kinds=+p \
	    --fields=+iaS --extra=+q

-include $(OBJS:.o=.d)
