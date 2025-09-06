#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/TransformedView.h"


#define WORLD_X			100
#define WORLD_Y			100

#define WORLD_ERROR 	-1
#define WORLD_INACTIVE	0
#define WORLD_ACTIVE	1


char* world;
char* world_target;

char World_Get(char* world,int x,int y){
	if(x<0 || x>=WORLD_X || y<0 || y>=WORLD_Y) return WORLD_ERROR;
	return world[y * WORLD_X + x];
}
void World_Set(char* world,int x,int y,char c){
	if(x<0 || x>=WORLD_X || y<0 || y>=WORLD_Y) return;
	world[y * WORLD_X + x] = c;
}
char World_Get_N(char* world,int x,int y){
	if(x<0 || x>=WORLD_X || y<0 || y>=WORLD_Y) return 0;
	
	char count = 0;
	for(int i = -1;i<=1;i++){
		for(int j = -1;j<=1;j++){
			if(i==0 && j==0) continue;
			
			const char c = World_Get(world,x + j,y + i);
			if(c == WORLD_ACTIVE) count++;
		}
	}
	return count;
}

TransformedView tv;

void Setup(AlxWindow* w){
	tv = TransformedView_New((Vec2){ GetWidth(),GetWidth() });
	TransformedView_Offset(&tv,(Vec2){ 0.0f,0.0f });

	world = (char*)malloc(sizeof(char) * WORLD_X * WORLD_Y);
	memset(world,0,sizeof(char) * WORLD_X * WORLD_Y);
	
	world_target = (char*)malloc(sizeof(char) * WORLD_X * WORLD_Y);
	memset(world_target,0,sizeof(char) * WORLD_X * WORLD_Y);
}
void Update(AlxWindow* w){
	tv.ZoomSpeed = (float)w->ElapsedTime;
	TransformedView_HandlePanZoom(&tv,window.Strokes,(Vec2){ GetMouse().x,GetMouse().y });
	
	const Vec2 tl = TransformedView_ScreenWorldPos(&tv,(Vec2){ 0.0f,0.0f });
	const Vec2 br = TransformedView_ScreenWorldPos(&tv,(Vec2){ GetWidth(),GetHeight() });
	
	if(Stroke(ALX_KEY_SPACE).DOWN){
		memset(world_target,0,sizeof(char) * WORLD_X * WORLD_Y);

		for(int i = (int)F32_Floor(tl.y) - 1;i<(int)F32_Ceil(br.y);i++){
			for(int j = (int)F32_Floor(tl.x) - 1;j<(int)F32_Ceil(br.x);j++){
				const char c = World_Get(world,j,i);
				const char count = World_Get_N(world,j,i);

				if(c == WORLD_ACTIVE){
					if(count<2 || count>3){
						World_Set(world_target,j,i,WORLD_INACTIVE);
					}else{
						World_Set(world_target,j,i,WORLD_ACTIVE);
					}
				}else{
					if(count==3){
						World_Set(world_target,j,i,WORLD_ACTIVE);
					}else{
						World_Set(world_target,j,i,WORLD_INACTIVE);
					}
				}
			}
		}

		memcpy(world,world_target,sizeof(char) * WORLD_X * WORLD_Y);
	}

	if(Stroke(ALX_MOUSE_L).DOWN){
		Vec2 m = TransformedView_ScreenWorldPos(&tv,GetMouse());
		World_Set(world,m.x,m.y,WORLD_ACTIVE);
	}
	if(Stroke(ALX_MOUSE_R).DOWN){
		Vec2 m = TransformedView_ScreenWorldPos(&tv,GetMouse());
		World_Set(world,m.x,m.y,WORLD_INACTIVE);
	}

	Clear(BLACK);
	
	for(int i = (int)F32_Floor(tl.y) - 1;i<(int)F32_Ceil(br.y);i++){
		for(int j = (int)F32_Floor(tl.x) - 1;j<(int)F32_Ceil(br.x);j++){
			const char c = World_Get(world,j,i);
			const Vec2 bg_p = TransformedView_WorldScreenPos(&tv,(Vec2){ j,i });
			const Vec2 bg_d = TransformedView_WorldScreenLength(&tv,(Vec2){ 1.0f,1.0f });
			
			if(c==WORLD_ACTIVE){
				RenderRect(bg_p.x,bg_p.y,bg_d.x + 1,bg_d.y + 1,GREEN);
			}else if(c==WORLD_INACTIVE){
				RenderRect(bg_p.x,bg_p.y,bg_d.x + 1,bg_d.y + 1,RED);
			}else{
				RenderRect(bg_p.x,bg_p.y,bg_d.x + 1,bg_d.y + 1,GRAY);
			}
		}
	}
}
void Delete(AlxWindow* w){
    if(world) free(world);
	world = NULL;
	
    if(world_target) free(world_target);
	world_target = NULL;
}

int main(){
    if(Create("Cellular Automa 1",2200,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}