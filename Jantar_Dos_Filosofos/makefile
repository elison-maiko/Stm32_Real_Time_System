# Variáveis
CXX = g++
CXXFLAGS = -std=c++11 -pthread -fsanitize=thread

# Alvo principal
all: jantar

# Como compilar o código e gerar o executável
jantar: Jantar.o
	$(CXX) $(CXXFLAGS) Jantar.o -o jantar

# Como compilar o arquivo objeto a partir do .cpp
Jantar.o: Jantar.cpp
	$(CXX) $(CXXFLAGS) -c Jantar.cppa