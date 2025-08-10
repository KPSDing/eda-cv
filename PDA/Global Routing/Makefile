# 編譯器和編譯選項
CXX = g++
CXXFLAGS = -march=x86-64 -flto -funroll-loops -finline-functions -ffast-math -o3 -Iinc -std=c++11 -fno-strict-aliasing -fopenmp

# 目標檔案和目錄
SRC_DIR = src
OBJ_DIR = obj

# 生成的可執行文件名稱
TARGET = D2DGRter

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
run0: $(TARGET)
	./$(TARGET) publicCase/testcase0/testcase0.gmp publicCase/testcase0/testcase0.gcl publicCase/testcase0/testcase0.cst publicCase/testcase0/testcase0.lg visualizer/testcase0.txt
	./Evaluator publicCase/testcase0 testcase0

run1: $(TARGET)
	./$(TARGET) publicCase/testcase1/testcase1.gmp publicCase/testcase1/testcase1.gcl publicCase/testcase1/testcase1.cst publicCase/testcase1/testcase1.lg visualizer/testcase1.txt
	./Evaluator publicCase/testcase1 testcase1

run2: $(TARGET)
	./$(TARGET) publicCase/testcase2/testcase2.gmp publicCase/testcase2/testcase2.gcl publicCase/testcase2/testcase2.cst publicCase/testcase2/testcase2.lg visualizer/testcase2.txt
	./Evaluator publicCase/testcase2 testcase2

runall: $(TARGET)
	./$(TARGET) publicCase/testcase0/testcase0.gmp publicCase/testcase0/testcase0.gcl publicCase/testcase0/testcase0.cst publicCase/testcase0/testcase0.lg visualizer/testcase0.txt
	./$(TARGET) publicCase/testcase1/testcase1.gmp publicCase/testcase1/testcase1.gcl publicCase/testcase1/testcase1.cst publicCase/testcase1/testcase1.lg visualizer/testcase1.txt
	./$(TARGET) publicCase/testcase2/testcase2.gmp publicCase/testcase2/testcase2.gcl publicCase/testcase2/testcase2.cst publicCase/testcase2/testcase2.lg visualizer/testcase2.txt
	./Evaluator publicCase/testcase0 testcase0
	./Evaluator publicCase/testcase1 testcase1
	./Evaluator publicCase/testcase2 testcase2
