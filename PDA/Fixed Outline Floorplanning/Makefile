# 編譯器和編譯選項
CXX = g++
CXXFLAGS = -march=native -flto -funroll-loops -finline-functions -ffast-math -O3 -Iinc -std=c++11

# 目標檔案和目錄
SRC_DIR = src
OBJ_DIR = obj

# 生成的可執行文件名稱
TARGET = Lab2

# 源碼檔案和對應的物件檔案
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) main.cpp
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES:main.cpp=$(OBJ_DIR)/main.o))

# .PHONY 表示不會生成相應的檔案
.PHONY: all clean

# 默認目標，生成可執行文件
all: $(TARGET)

# 連結所有物件檔案生成可執行檔案
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# 編譯.cpp檔案到.o檔案
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 編譯 main.cpp
$(OBJ_DIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o $@

# 清理生成的檔案
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# 測試指令
run33: $(TARGET)
	./$(TARGET) 0.5 testcase/ami33/ami33.block testcase/ami33/ami33.nets output/output33.txt output/output33_draw.txt
	#python3 draw_block_layout.py output/output33_draw.txt drawing/output33_draw.png

run49: $(TARGET)
	./$(TARGET) 0.5 testcase/ami49/ami49.block testcase/ami49/ami49.nets output/output49.txt output/output49_draw.txt
	#python3 draw_block_layout.py output/output49_draw.txt drawing/output49_draw.png

run317b: $(TARGET)
	./$(TARGET) 0.5 testcase/vda317b/vda317b.block testcase/vda317b/vda317b.nets output/output317b.txt output/output317b_draw.txt
	#python3 draw_block_layout.py output/output317b_draw.txt drawing/output317b_draw.png

runfastsp:
	./$(TARGET) 0.5 testcase/fastsp/fastsp.block testcase/fastsp/fastsp.nets output/outputfastsp.txt output/outputfastsp_draw.txt
	#python3 draw_block_layout.py output/outputfastsp_draw.txt drawing/outputfastsp_draw.png

