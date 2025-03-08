#include <iostream>
#include <vector>
#include <time.h>
#include <thread>
#include <algorithm>
#include <queue>
#include <SDL2/SDL.h>

//Measurements of the window
const int WIDTH = 600, HEIGHT = 600;
//Radius of drawn vertexes
const int VRAD= 10;
//Indexes of chosen points
int first = -1, second = -1;
//Bools for multithreading
bool dijkstraMode = false, dijkstraWorking = false;
//Window title
const char *mainTitle = "Grafik"; 
//Window
SDL_Window *mainWindow = nullptr;
//Renderer
SDL_Renderer *renderer = nullptr;
//Checking if point is closer than VRAD to another point
bool CheckCircle(int x1, int y1, int x2, int y2){
    if((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)<=VRAD*VRAD){
        return true;
    }
    return false;
}
//Vertex structure needed for visualising
struct _vertex{
    int x, y, index;   
};
//Drawing circle around vertex
void DrawCircle(_vertex v){
    for(int i= v.x-VRAD+1; i<=v.x+VRAD; i++){
        for(int j= v.y-VRAD+1; j<=v.y+VRAD; j++){
            if(CheckCircle(v.x, v.y, i, j)){
                SDL_RenderDrawPoint(renderer, i,j);
            }
        }
    }
}
//List of vertexes 
std::vector< _vertex > vertexes;
//Adjacency matrix
std::vector< std::vector<float> > edges;
//List of edges to color
std::vector< std::pair<int ,int> > coloredEdges;
//List of vertexes to color
std::vector<int> coloredVertexes;
//Function for setting up the window and renderer
void WindowSetup(){
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &mainWindow, &renderer);
    SDL_RenderSetScale(renderer,1 ,1);
    SDL_SetWindowTitle(mainWindow, mainTitle);
}
//Function for drawing vertexes with adequate color
void DrawVertexes(){
    for(_vertex v: vertexes){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(int in : coloredVertexes){
            if(in==v.index){
                SDL_SetRenderDrawColor(renderer, 100, 200, 200, 255);
            }
        }
        if(v.index == first||v.index == second){
            SDL_SetRenderDrawColor(renderer, 100, 240, 50, 255);
            if(dijkstraMode){
                SDL_SetRenderDrawColor(renderer, 206, 233, 30, 255);
            }
        }
        DrawCircle(v);
    }
}
//Function for drawing edges with adequate color
void DrawEdges(){
    
    for(int i=0; i<edges.size();i++){
        for(int j=0; j<edges[i].size(); j++){
            if(edges[i][j]!=-1.0f){
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                for(int k=0; k<coloredEdges.size(); k++){
                    if((coloredEdges[k].first ==i &&coloredEdges[k].second == j)||(coloredEdges[k].first == j&&coloredEdges[k].second ==i)){
                        SDL_SetRenderDrawColor(renderer, 100, 150, 230, 255);
                    }
                }
                SDL_RenderDrawLine(renderer, vertexes[i].x, vertexes[i].y, vertexes[j].x, vertexes[j].y);
            }
        }
    }
}
//Function for calculating distance between two vertexes
float EdgeLen(_vertex a, _vertex b){
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
}
//Function to add edges between two vertexes(deletes the edge if it already exists)
void AddEdge(_vertex a, _vertex b){
    if(a.index==b.index){
        return;
    }
    if(edges[a.index][b.index]!=-1.0f){
        edges[a.index][b.index] = -1.0f;
        edges[b.index][a.index] = -1.0f;
        return;
    }
    float len = EdgeLen(a,b);
    edges[a.index][b.index] = len;
    edges[b.index][a.index] = len;
}
//Evens out adjacency matrix after adding a vertex
void EvenAdjMatrix(){
    for(int i=0; i<edges.size(); i++){
        while(edges[i].size()<edges.size()){
            edges[i].push_back(-1.0f);
        }
    }
}
//Function for adding a vertex
void AddVertex(int x, int y){
    if(vertexes.size()>0){
        for(_vertex ver: vertexes){
            if(ver.x == x&&ver.y==y){
                return;
            }
        }
    }
    _vertex v;
    v.x = x;
    v.y = y;
    v.index = vertexes.size();
    vertexes.push_back(v);
    std::vector<float> s;
    edges.push_back(s);
    EvenAdjMatrix();
}
//Function for deleting vertexes
void DeleteVertex(int n){
    edges.erase(edges.begin()+n);
    for(int i=0; i<edges.size(); i++){
        edges[i].erase(edges[i].begin()+n);
    }
    vertexes.erase(vertexes.begin()+n);
    for(int i=0; i<vertexes.size(); i++){
        vertexes[i].index = i;
    }
    first =-1;
}
//Dijkstras algorithm loop for finding the shortest path between two vertexes and visualising it
void Dijkstra(){
    while(true){
        if(dijkstraMode&&first!=-1&&second!=-1){
            dijkstraWorking = true;
            int n, start, end;
            start = first;
            end = second;
            n = vertexes.size();
            std::vector<float> distances(n+1, FLT_MAX);
            std::vector<int> predecessors(n+1, -1);
            distances[start]=0.0;
            std::priority_queue< std::pair<float , int> > Q;
            Q.push(std::make_pair(0.0, start));
            while(!Q.empty()){
                int v = Q.top().second;
                Q.pop();
                coloredVertexes.push_back(v);
                for(int i=0; i<edges[v].size(); i++){
                    if(edges[v][i]!=-1.0f){
                        SDL_Delay(100);
                        coloredEdges.push_back(std::make_pair(v,i));
                        float w = edges[v][i];
                        if(distances[v]+w<distances[i]){
                            distances[i]=distances[v]+w;
                            predecessors[i]=v;
                            Q.push(std::make_pair(-w, i));
                        }
                    }
                }
            }
            std::vector<int> path;
            int c=end;
            while(predecessors[c]!=-1){
                c=predecessors[c];
                path.push_back(c);
            }
            reverse(path.begin(), path.end());
            SDL_Delay(10);
            std::vector< std::pair<int , int> > edg;
            std::vector<int> ver;
            coloredEdges = edg;
            coloredVertexes = ver;
            int prev = -1;
            for(int in: path){
                SDL_Delay(100);
                if(in ==-1){
                    dijkstraMode = false;
                    coloredEdges = edg;
                    coloredVertexes = ver;
                    break;
                }
                coloredEdges.push_back(std::make_pair(prev, in));
                coloredVertexes.push_back(in);
                prev = in;
                
            }
            coloredEdges.push_back(std::make_pair(prev, end));
            coloredVertexes.push_back(end);
            
            first = -1;
            second = -1;
            while(dijkstraMode){
            }
            coloredEdges = edg;
            coloredVertexes = ver;
            dijkstraWorking = false;
        }
    }
}
//Main loop to project the graph and change it
void Loop(){
    bool var = true;
    while(var){
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_Event event;
        if(SDL_PollEvent(&event)){
            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_ESCAPE:{
                        var = false;
                        break;
                    }
                    case SDLK_d:{
                        dijkstraMode = !dijkstraMode;
                        break;
                    }
                }
            }
            if(event.type == SDL_QUIT){
                    var = false;
                    return;
            } 
            if(event.type == SDL_MOUSEBUTTONDOWN){
                int x, y;
                bool clickedVertex = false;
                SDL_GetMouseState(&x, &y);
                for(_vertex v: vertexes){
                    clickedVertex = CheckCircle(x, y, v.x, v.y);
                    if(clickedVertex&&!dijkstraWorking){
                        switch(event.button.button){
                            case SDL_BUTTON_LMASK:{
                                
                                    if(first == -1){
                                        first = v.index;
                                    }
                                    else{
                                        second = v.index;
                                        if(!dijkstraMode){
                                        AddEdge(vertexes[first], vertexes[second]);
                                        first = -1;
                                        second = -1;
                                        }
                                    }
                                break;
                            }
                            case SDL_BUTTON_RIGHT:{
                                
                                DeleteVertex(v.index);
                                break;
                            }
                        }
                        break;
                    }
                }
                if(!clickedVertex){
                    switch(event.button.button){
                        case SDL_BUTTON_LMASK:{
                            AddVertex(x,y);
                            break;
                        }
                    }
                }               
            }
        }
        DrawEdges();
        DrawVertexes();
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderPresent(renderer);
    }
}

int main(){
    WindowSetup();
    std::thread dijkstra(Dijkstra);
    Loop();
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
    return EXIT_SUCCESS;
}