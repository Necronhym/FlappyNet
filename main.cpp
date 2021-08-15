#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

//Window Paramaters
int WIDTH=800;
int HEIGHT=800;
const char* NAME = "Flappy Net";
//Phisics cosntants:
const float DEG2RAD = 3.14159/180;
float birdAcc = -0.0003;

//Random double between -1.0 and 1.0
class NeuralNet
	{
		std::vector<unsigned> Topology;
		
		std::vector<std::vector<double>> NeuronLayers;
		std::vector<std::vector<double>> BiasLayers;
		std::vector<std::vector<std::vector<double>>> ConnectionLayers;
	
		double randomDouble()
			{
				return -1.0 + ((double)rand() / RAND_MAX) * 2.0;
			}
		double sigmoid(double x)
			{
				return (exp(x) - exp(-x)) / (exp(x) + exp(-x));
			}
		/*
		 *	ANN Layout:
		 *	Layer0[N] ConectionLayero[N][M] Layer1[M] ConnectionLayer1[M][O] Layer[O} ...
		 *		  BiasLayer0[M]			  BiasLayer1[O]
		 */

		void initiateRandomNetwork()
			{
				for(int i=0; i<Topology.size(); i++)
					{
						//Initiate Neurons
						std::vector<double> NeuronLayer;
						for(int j=0; j<Topology.at(i); j++)
							{
								NeuronLayer.push_back(randomDouble());	
							}
						NeuronLayers.push_back(NeuronLayer);
						
						//Initate Biases
						std::vector<double> BiasLayer;
						for(int j=0; j<Topology.at(i); j++)
							{
								BiasLayer.push_back(randomDouble());	
							}
						BiasLayers.push_back(BiasLayer);

						//So as to not create connection layer to nonexistant layer
						if( i == Topology.size()-1 )
							{
								break;
							}
						std::vector<std::vector<double>> ConnectionLayer;
						for(int j=0; j<Topology.at(i); j++)
							{
								std::vector<double> CLayer;
								for(int k=0; k<Topology.at(i+1); k++)
									{
										CLayer.push_back(randomDouble());
									}
								ConnectionLayer.push_back(CLayer);
							}
						ConnectionLayers.push_back(ConnectionLayer);
					}

			}
		public:
		NeuralNet(std::vector<unsigned> Topology)
			{
				this->Topology = Topology;
				initiateRandomNetwork();
			}
		void feedForward(std::vector<double> Input)
			{
				NeuronLayers.at(0) = Input;
				for(int i=1; i<Topology.size(); i++)
					{
						for(int j=0; j<Topology.at(i); j++)
							{
								double sum=0;
								for(int k=0; k<Topology.at(i-1); k++)
									{
										sum+=NeuronLayers.at(i-1).at(k)
										*ConnectionLayers.at(i-1).at(k).at(j);
									}
								NeuronLayers.at(i).at(j)= sigmoid(sum + BiasLayers.at(i).at(j));
							}
					}
			}
		std::vector<double> getOutputs()
			{
				return NeuronLayers.back();
			}
		//GA preperation:
		std::vector<unsigned> getTopology()
			{
				return Topology;
			}
		std::vector<std::vector<std::vector<double>>> getWeightGene()
			{
				return ConnectionLayers;
			}
		std::vector<std::vector<double>> getBiasGene()
			{
				return BiasLayers;
			}
		void setGenes(std::vector<std::vector<std::vector<double>>> ConnnectionLayers,
				std::vector<std::vector<double>> BiasLayers)
			{
				this->ConnectionLayers = ConnectionLayers;
				this->BiasLayers = BiasLayers;
			}
	};

class GeneticAlgorithm
	{
		std::vector<unsigned> NeuralNetworkTopology;
		unsigned mutationRate;
		unsigned crossoverRate;
		
		double randomDouble()
			{
				return -1.0 + ((double)rand() / RAND_MAX) * 2.0;
			}
		
		public:
		GeneticAlgorithm(std::vector<unsigned> Topology, unsigned mutationRate, unsigned crossoverRate)
			{
				this->NeuralNetworkTopology = Topology;
				this->mutationRate = mutationRate;
				this->crossoverRate = crossoverRate;
			}
		std::vector<std::vector<std::vector<double>>> connectionMutation(
				std::vector<std::vector<std::vector<double>>> OldConnectionLayers)
			{
				std::vector<std::vector<std::vector<double>>> NewConnectionLayers;
				for(int i=0; i<NeuralNetworkTopology.size(); i++)
					{
						std::vector<std::vector<double>> ConnectionLayer;
						for(int j=0; j<NeuralNetworkTopology.at(i)-1; j++)
							{
								std::vector<double> Layer;
								for(int k=0; k<NeuralNetworkTopology.at(i+1); k++)
									{
										if(rand()%101+1 < mutationRate)
											{
												Layer.push_back(randomDouble());
											}
										else
											{
												Layer.push_back(OldConnectionLayers.at(i).at(j).at(k));
											}
									}
								ConnectionLayer.push_back(Layer);
							}
					}	
				return NewConnectionLayers;
			}
		//WORKS
		std::vector<std::vector<double>> biasMutation(std::vector<std::vector<double>> OldBiasLayers)
			{
				std::vector<std::vector<double>> NewBiasLayers;
				for(int i=0; i<NeuralNetworkTopology.size(); i++)
					{
						//Initate Biases
						std::vector<double> Layer;
						for(int j=0; j<NeuralNetworkTopology.at(i); j++)
							{
								if(rand()%101+1 < mutationRate)
									{
										Layer.push_back(randomDouble());
									}
								else
									{
										Layer.push_back(OldBiasLayers.at(i).at(j));
									}
							}
						NewBiasLayers.push_back(Layer);
					}

				return NewBiasLayers;
			}

		std::vector<std::vector<std::vector<double>>> connectionCrossover(
				std::vector<std::vector<std::vector<double>>> ConnectionLayers1,
				std::vector<std::vector<std::vector<double>>> ConnectionLayers2)
			{
				std::vector<std::vector<std::vector<double>>> NewConnectionLayers;
				for(int i=0; i<NeuralNetworkTopology.size(); i++)
					{
						std::vector<std::vector<double>> ConnectionLayer;
						for(int j=0; j<NeuralNetworkTopology.at(i)-1; j++)
							{
								std::vector<double> Layer;
								for(int k=0; k<NeuralNetworkTopology.at(i+1); k++)
									{
										if(rand()%101+1 < crossoverRate)
											{
												Layer.push_back(ConnectionLayers1.at(i).at(j).at(k));
											}
										else
											{
												Layer.push_back(ConnectionLayers2.at(i).at(j).at(k));
											}
									}
								ConnectionLayer.push_back(Layer);
							}
					}	
				return NewConnectionLayers;
			}
		std::vector<std::vector<double>> biasCrossover
			(std::vector<std::vector<double>> BiasLayerOne,
			 std::vector<std::vector<double>> BiasLayerTwo)
			{
				std::vector<std::vector<double>> NewBiasLayers;
				for(int i=0; i<NeuralNetworkTopology.size(); i++)
					{
						//Initate Biases
						std::vector<double> Layer;
						for(int j=0; j<NeuralNetworkTopology.at(i); j++)
							{
								if(rand()%101+1 < crossoverRate)
									{
										Layer.push_back(BiasLayerTwo.at(i).at(j));
									}
								else
									{
										Layer.push_back(BiasLayerOne.at(i).at(j));
									}
							}
						NewBiasLayers.push_back(Layer);
					}

				return NewBiasLayers;
			}
	};

struct color
	{
		double r,g,b,a;
		color(double r = 0.5, double g = 0.5, double b=0.5, double a=0.5)
			{
				this->r = r;
				this->g = g;
				this->b = b;
				this->a = a;
			}
		}RED(1,0,0,1),GREEN(0,1,0,1),BLUE(0,0,1,1), YELLOW(1,1,0,1);
class Pipe
	{
		double x, y, velX;
		void drawRectangle(float x, float y, float w, float h, color C = color())
			{
				glBegin(GL_QUADS);
				glColor4d(C.r, C.g, C.b, C.a);
				glVertex3f(x, y, 1);
				glColor4d(C.r, C.g, C.b, C.a);
				glVertex3f(x+w, y, 1);
				glColor4d(C.r, C.g, C.b, C.a);
				glVertex3f(x+w, y+h, 1);
				glColor4d(C.r, C.g, C.b, C.a);
				glVertex3f(x, y+h, 1);
				glEnd();
			}
		public:
		Pipe(float x, float y)
			{
				this->velX = -0.005;
				this->x = x;
				this->y = y;
			}
		void draw()
			{
				float xc = this->x*2.0 - 1.0;
				float yc = this->y*2.0 - 1.0;
				drawRectangle(xc, yc+0.25, 0.25, 2, color(0.0, 0.8, 0.2, 0.5));
				drawRectangle(xc, yc-2.25, 0.25, 2, color(0.0, 0.8, 0.2, 0.5));
			}
		void update()
			{
				x = x+velX;
				if(x < -0.2)
					{
						x = 1.0;
						y = 0.2 + ((float)rand()/(float)(RAND_MAX))* 0.6;
					}
			}
		double getX(){return x;}
		double getY(){return y;}
	};
class Bird
	{
		double x, y, velY;
		double pipeX, pipeY;
		bool Alive;
		int fitness;
		std::vector<unsigned> Topology{4,16,2,1};
		NeuralNet N = NeuralNet(Topology);
		color C;
		void drawCircle(float x, float y, float radius, color C = color())
			{
				glBegin(GL_POLYGON);
		 		for (int i=0; i < 360; i++)
					{
						float degInRad = i*DEG2RAD;
						glColor4d(C.r, C.g, C.b, C.a);
						glVertex3d(cos(degInRad)*radius + x,sin(degInRad)*radius + y, 1);
					} 
				glEnd();
			}
		public:
		Bird(double x, double y, color C, bool Alive)
			{
				this->velY = 0.0;
				this->x = x;
				this->y = y;
				this->C = C;
				this->Alive = Alive;
				this->fitness = 0;
			}
		void setColor(color C)
			{
				this->C = C;
			}
		void draw()
			{
				float xc = x*2.0 - 1.0;
				float yc = y*2.0 - 1.0;
				if(Alive) drawCircle(xc, yc, 0.05, C);
			}
		void setPipe(double x, double y)
			{
				this->pipeX=x;
				this->pipeY=y;
			}
		void flap()
			{
				velY = 0.01;
			}
		void update()
			{
				velY += birdAcc;
				y += velY;
				if(Alive)
					{
						std::vector<double> inputs = 
						{pipeX-0.25, y, pipeY-y+ 0.1, pipeY-y -0.1};
						fitness++;
						N.feedForward(inputs);
						if(N.getOutputs().at(0)>=0.0)
							{
								flap();
							}
					}
			}
		void setBird(double x, double y, color C, bool Alive)
			{
				this->velY = 0.0;
				this->x = x;
				this->y = y;
				this->C = C;
				this->Alive = Alive;
				this->fitness = 0;
			}
		double getX(){return x;}
		double getY(){return y;}
		void setAlive(bool A)
			{
				this->Alive = A;
			}
		bool isAlive()
			{
				return this->Alive;
			}
		int getFitness()
			{
				return this-> fitness;
			}
		void setFitness(int i)
			{
				this->fitness = i;
			}
		std::vector<unsigned> getTopology()
			{
				return this->Topology;
			}

		//Genetic Algorithm setup
		std::vector<std::vector<std::vector<double>>> getWeightGene()
			{
				return N.getWeightGene();
			}
		std::vector<std::vector<double>> getBiasGene()
			{
				return N.getBiasGene();
			}
		void setGenes(std::vector<std::vector<std::vector<double>>> ConnnectionLayers,
				std::vector<std::vector<double>> BiasLayers)
			{
				N.setGenes(ConnnectionLayers, BiasLayers);
			}
		//SortingAlgorithm prep
		bool operator< (const Bird &o) const
			{
				return fitness < o.fitness;
			}
	};
void colision(std::vector<Bird> &Birds, std::vector<Pipe> &Pipes)
	{
		for(auto &B : Birds)
			{
				B.setPipe(0.7, 0.5);
				for(auto P: Pipes)
					{
						if(P.getX() > 0.10 && P.getX() < 0.70)
							{
								B.setPipe(P.getX(), P.getY());
								if( B.getX() > (P.getX() - 0.0175) &&
								B.getX() < (P.getX() + 0.150) &&(
								B.getY() < P.getY() - 0.1 ||
								B.getY() > P.getY() + 0.1 ))
									{
										B.setAlive(0);
										break;
									}
							}
					}
				if(B.getY() < 0.025)
					{
						B.setAlive(0);
					}
			}
	}
std::vector <Pipe> Pipes;
std::vector <Bird> Birds;
void init()
	{
		glClearColor(0.2,0.6,0.9,1);
		Pipes.clear();
		Pipes.push_back(Pipe(1.0, 0.5));
		Pipes.push_back(Pipe(1.6, 0.5));
	}
void initBirds(int P=250)
	{
		int survivalRate=50;
		int reproductionRate=100;
		int mutationRate=150;
		if(Birds.empty())
			{
				for(int i=0; i<P; i++)
					{
						Birds.push_back(Bird(0.25, 0.5, YELLOW, true));
					}
				return;
			}
		else
			{
				GeneticAlgorithm GA = GeneticAlgorithm(Birds.at(0).getTopology(), 25, 50);
				std::sort(Birds.begin(), Birds.end());
				std::reverse(Birds.begin(), Birds.end());
				std::vector<Bird> NewBirds;
				NewBirds.clear();
				int i=0;
				for(int i=0; i<Birds.size(); i++)
					{
						if(i<survivalRate)
							{
								Birds.at(i).setBird(0.25, 0.5, RED, true);
								NewBirds.push_back(Birds.at(i));
								continue;
							}
						else if(i<reproductionRate)
							{

								int b1=rand()%survivalRate;
								int b2=rand()%survivalRate;
								while(b2 == b1)
									{
										b2=rand()%survivalRate;
									}
								Birds.at(i).setGenes(
								GA.connectionCrossover(	
									Birds.at(b1).getWeightGene(),
									Birds.at(b2).getWeightGene()),
								GA.biasCrossover(
									Birds.at(b1).getBiasGene(),
									Birds.at(b2).getBiasGene()
									));
								
								Birds.at(i).setBird(0.25, 0.5, GREEN, true);
								NewBirds.push_back(Birds.at(i));
							}
						else if(i<mutationRate)
							{
								int b=rand()%survivalRate;
								Birds.at(b).setGenes(
								GA.connectionMutation(Birds.at(b).getWeightGene()),
								GA.biasMutation(Birds.at(b).getBiasGene()));
								Birds.at(b).setBird(0.25, 0.5, BLUE, true);
								NewBirds.push_back(Birds.at(b));
							}
						else
							{	
								NewBirds.push_back(Bird(0.25, 0.5, YELLOW, true));
							}
					}
				Birds.clear();
				Birds = NewBirds;
			}
	}
void display()
	{
		int LivingBirds=0;
		for(auto &B: Birds)
			{
				B.update();
				if(B.isAlive())
					{
						LivingBirds++;
					}
			}
		for(auto &P : Pipes)
			{
				P.update();
			}
		colision(Birds, Pipes);	
		//Render:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for(auto P : Pipes)
			{
				P.draw();
			}
		for(auto &B: Birds)
			{
				B.draw();
			}
		if(LivingBirds == 0)
			{
				init();
				initBirds();
			}
		//Redraw
		glutSwapBuffers();
		glutPostRedisplay();	
	}
void keyboard(unsigned char key, int m, int n)
	{
		switch(key)
			{
	/*			case 32:
					for(auto &B: Birds)
						{
							B.flap();
						}
				break;
	*/			case 27:
					exit(0);
				break;

			}
	}
int main(int argc, char* argv[])
	{

		srand(time(NULL));
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

		glutInitWindowPosition(200, 200);
		glutInitWindowSize(WIDTH, HEIGHT);
		glutCreateWindow(NAME);

		init();
		initBirds();
		glutDisplayFunc(display);
		glutKeyboardFunc(keyboard);

		glutMainLoop();

		return 0;
	}
