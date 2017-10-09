#include "GameLevel.h"
#include "Base/Debug.h"
#include "Base/ResourceManager.h"

#include <fstream>
#include <sstream>

GameLevel::GameLevel()
{
	bricks = std::vector<GameObject>();
}

GameLevel::~GameLevel()
{
	bricks.clear();
}

void GameLevel::Draw( SpriteRenderer* renderer )
{
	for( int i = 0; i < bricks.size(); i++ )
	{
		if( !bricks[i].isDestroyed )
			bricks[i].Draw(renderer);
	}
}

bool GameLevel::IsCompleted()
{
	for( int i = 0; i < bricks.size(); i++ )
	{
		if( !bricks[i].isSolid && !bricks[i].isDestroyed )
			return false;
	}
	
	return true;
}

void GameLevel::Reset()
{
	for( GameObject &brick : bricks )
	{
		brick.isDestroyed = false;
	}
}

bool GameLevel::Load( const char* file, int width, int height, glm::vec2 posOffset )
{
	bricks.clear();

	std::string line;
	std::ifstream fstream( file );
	std::vector<std::vector<int>> tileData;
	int number;

	if( fstream ){
		while( std::getline(fstream, line) )
		{
			std::vector<int> row;

			std::istringstream sstream( line );			
			while( sstream >> number ) // Read each word seperated by spaces
				row.push_back( number );

			tileData.push_back( row );
		}

		if( tileData.size() > 0 )
			return Init( tileData, width, height, posOffset );
		else
			Debug::LogError("No tile data!!!");
	}
	return false;
}

bool GameLevel::Init( std::vector<std::vector<int>> data, int width, int height, glm::vec2 posOffset )
{
	int row = data.size();
	int column = data[0].size();

	float unitWidth = (float)width / column;
	float unitHeight = (float)height / row;

	int tileType = 0;
	for( int r = 0; r < row; r++ )
	{
		for( int c = 0; c < column; c++ )
		{
			glm::vec2 pos(unitWidth*c + posOffset.x, height-unitHeight- unitHeight*r + posOffset.y );
			glm::vec2 size( unitWidth, unitHeight );

			tileType = data[r][c];
			if( tileType == 1 )
			{
				glm::vec3 color( 0.8f, 0.8f, 0.7f );
				GameObject go(pos, size, ResourceManager::getInstance()->getTexture("block_solid"), color );
				go.isSolid = true;

				bricks.push_back(go);
			}
			else if( tileType > 1 )
			{
				glm::vec3 color( 1.0 );
				if( tileType == 2 )
					color = glm::vec3( 0.2f, 0.6f, 1.0f );
				else if( tileType == 3 )
					color = glm::vec3( 0.0f, 0.7f, 0.0f );
				else if( tileType == 4 )
					color = glm::vec3( 0.8f, 0.8f, 0.4f );
				else if( tileType == 5 )
					color = glm::vec3( 1.0f, 0.5f, 0.0f );

				GameObject go( pos, size, ResourceManager::getInstance()->getTexture( "block" ), color );

				bricks.push_back( go );
			}
		}
	}

	return true;
}