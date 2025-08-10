# 編譯器和編譯選項
CXX = g++
CXXFLAGS = -march=x86-64 -flto -funroll-loops -finline-functions -ffast-math -O3 -Iinc -std=c++11 -fno-strict-aliasing  -fopenmp

# 目標檔案和目錄
SRC_DIR = src
OBJ_DIR = obj

# 生成的可執行文件名稱
TARGET = Legalizer

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
run16900: $(TARGET)
	./$(TARGET) testcase/testcase1_16900.lg testcase/testcase1_16900.opt output/testcase1_16900_post.lg visualizer/output_16900_gif.txt visualizer/output_16900_draw.txt 
	./Evaluator testcase/testcase1_16900.lg testcase/testcase1_16900.opt output/testcase1_16900_post.lg
	python3 visualize_flip_flops.py visualizer/output_16900_draw.txt visualizer/output_16900_draw.png

run5000: $(TARGET)
	./$(TARGET) testcase/testcase1_ALL0_5000.lg testcase/testcase1_ALL0_5000.opt output/testcase1_ALL0_5000_post.lg visualizer/output_ALL0_5000_gif.txt visualizer/output_ALL0_5000_draw.txt
	./Evaluator testcase/testcase1_ALL0_5000.lg testcase/testcase1_ALL0_5000.opt output/testcase1_ALL0_5000_post.lg
	python3 visualize_flip_flops.py visualizer/output_ALL0_5000_draw.txt visualizer/output_ALL0_5000_draw.png

run100: $(TARGET)
	./$(TARGET) testcase/testcase2_100.lg testcase/testcase2_100.opt output/testcase2_100_post.lg visualizer/output_100_gif.txt visualizer/output_100_draw.txt
	./Evaluator testcase/testcase2_100.lg testcase/testcase2_100.opt output/testcase2_100_post.lg
	python3 visualize_flip_flops.py visualizer/output_100_draw.txt visualizer/output_100_draw.png
 
run7000: $(TARGET)
	./$(TARGET) testcase/testcase1_MBFF_LIB_7000.lg testcase/testcase1_MBFF_LIB_7000.opt output/testcase1_MBFF_LIB_7000_post.lg visualizer/output_MBFF_LIB_7000_gif.txt visualizer/output_MBFF_LIB_7000_draw.txt
	./Evaluator testcase/testcase1_MBFF_LIB_7000.lg testcase/testcase1_MBFF_LIB_7000.opt output/testcase1_MBFF_LIB_7000_post.lg
	python3 visualize_flip_flops.py visualizer/output_MBFF_LIB_7000_draw.txt visualizer/output_MBFF_LIB_7000_draw.png

