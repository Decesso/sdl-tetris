#include "View.hh"
#include <stdexcept>
#include <SDL_image.h>
#include <string>

static const unsigned table1X = 18;
static const unsigned table1Y = 24;
static const unsigned table2X = 577;
static const unsigned table2Y = 24;
static const unsigned tableWidth = 385;
static const unsigned tableHeight = 595;

View::View(std::shared_ptr<LogicInterface> logic1, std::shared_ptr<LogicInterface> logic2, Character& player1Character, Character& player2Character)
: ViewInterface(logic1, logic2)
, character1_(player1Character)
, character2_(player2Character)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		throw std::runtime_error(SDL_GetError());
	}
	window_ = SDL_CreateWindow("Tetris", 100, 100, 1120, 640, SDL_WINDOW_SHOWN);
	if (window_ == nullptr) {
		throw std::runtime_error(SDL_GetError());
	}
	ren_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren_ == 0) {
		throw std::runtime_error(SDL_GetError());
	}
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		throw std::runtime_error(SDL_GetError());
	}
	background_ = IMG_LoadTexture(ren_, "img/background.png");
	if (background_ == 0) {
		throw std::runtime_error(SDL_GetError());
	}
	TTF_Init();
	font_ = TTF_OpenFont("sample.ttf", 20);
	blockTexture_ = IMG_LoadTexture(ren_, "img/blocks.png");
	if (blockTexture_ == 0) {
		throw std::runtime_error(SDL_GetError());
	}
	player1Avatar_ = IMG_LoadTexture(ren_, player1Character.getAvatar());
	if (player1Avatar_ == 0) {
		throw std::runtime_error(SDL_GetError());
	}
	player2Avatar_ = IMG_LoadTexture(ren_, player2Character.getAvatar());
	if (player2Avatar_ == 0) {
		throw std::runtime_error(SDL_GetError());
	}
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		throw std::runtime_error(SDL_GetError());
	}
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	/*bgMusic_ = Mix_LoadMUS("music/bg.mp3");
	if (bgMusic_ == 0)
		throw std::runtime_error(SDL_GetError());
	if (Mix_PlayMusic(bgMusic_, -1) == -1)
		throw std::runtime_error(SDL_GetError());*/
}

View::~View() {
	//Mix_FreeMusic(bgMusic_);
	SDL_DestroyTexture(blockTexture_);
	TTF_CloseFont(font_);
	SDL_DestroyTexture(background_);
	SDL_DestroyRenderer(ren_);
	SDL_DestroyWindow(window_);
}

SDL_Rect
getSect(Color color)
{
	SDL_Rect sect;
	sect.y = 0;
	sect.w = 50;
	sect.h = 50;
	switch (color) {
	case Color::yellow :
		sect.x = 50;
		break;
	case Color::green:
		sect.x = 0;
		break;
	case Color::grey:
		sect.x = 100;
		break;
	case Color::blue:
		sect.x = 150;
		break;
	case Color::red:
		sect.x = 200;
		break;
	case Color::purple:
		sect.x = 250;
		break;
	case Color::magic:
		sect.x = 300;
		break;
	case Color::none:
		assert(false);
		break;
	}
	return sect;
}

SDL_Rect
getDest(unsigned row, unsigned col, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Rect dest;
	dest.x = topleftX + (tableWidth / TETRIS_ROW) * row;
	dest.y = topleftY + (tableHeight / TETRIS_COL) * col;
	dest.w = (tableWidth / TETRIS_ROW);
	dest.h = (tableHeight / TETRIS_COL);
	return dest;
}
void
View::renderTable(std::shared_ptr<LogicInterface> logic, SDL_Texture* tex, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Rect sect;
	SDL_Rect dest;
	auto table = logic->getTableWithShape();
	for (size_t i = 0; i < table.size(); i++) {
		for (size_t j = 0; j < table.at(i).size(); j++) {
			if (table[i][j] == Color::none) continue;
			sect = getSect(table[i][j]);
			dest = getDest(j, i, topleftX, topleftY);
			SDL_RenderCopy(ren_, tex, &sect, &dest);
		}
	}
}

void
View::renderNextShape(std::shared_ptr<LogicInterface> logic, SDL_Texture* tex, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Rect sect;
	SDL_Rect dest;
	dest.w = tableWidth / TETRIS_ROW / 2;
	dest.h = tableHeight / TETRIS_COL / 2;
	auto shape = logic->getNextShape();
	if (!shape) return;
	for (size_t i = 0; i < shape->shape.size(); i++) {
		for (size_t j = 0; j < shape->shape.at(i).size(); j++) {
			if (shape->shape[i][j] == Color::none) continue;
			sect = getSect(shape->shape[i][j]);
			dest.x = topleftX + dest.w * j + 400;
			dest.y = topleftY + dest.h * i + 30;
			SDL_RenderCopy(ren_, tex, &sect, &dest);
		}
	}
}
void
View::renderMana(std::shared_ptr<LogicInterface> logic, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Rect destination;
	destination.x = topleftX;
	destination.y = topleftY;
	renderText(color, destination, std::to_string(logic->getMana()));
}

void
View::renderWins(std::shared_ptr<LogicInterface> logic, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Rect destination;
	destination.x = topleftX;
	destination.y = topleftY;
	renderText(color, destination, std::to_string(logic->gamesWon()));
}

void
View::renderAvatar(SDL_Texture* texture, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Rect dest;
	dest.x = topleftX;
	dest.y = topleftY;
	dest.w = 100;
	dest.h = 150;
	SDL_RenderCopy(ren_, texture, NULL, &dest);
}

void
View::renderSpecial(Character& character, const unsigned topleftX, const unsigned topleftY)
{
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Rect destination;
	destination.x = topleftX;
	destination.y = topleftY;
	auto specials = character.getSpecials();
	for (auto special : specials) {
		renderText(color, destination, special);
		destination.y += 20;
	}
}

void
View::render()
{
	SDL_RenderClear(ren_);
	SDL_RenderCopy(ren_, background_, NULL, NULL);

	renderTable(logicPlayer1_, blockTexture_, table1X, table1Y);
	renderNextShape(logicPlayer1_, blockTexture_, table1X, table1Y);

	renderTable(logicPlayer2_, blockTexture_, table2X, table2Y);
	renderNextShape(logicPlayer2_, blockTexture_, table2X, table2Y);

	renderMana(logicPlayer1_, 420, 150);
	renderMana(logicPlayer2_, 420 + table2X - table1X, 150);

	renderWins(logicPlayer1_, 420, 225);
	renderWins(logicPlayer2_, 420 + table2X - table1X, 225);

	renderAvatar(player1Avatar_, 410, 290);
	renderAvatar(player2Avatar_, 409 + table2X - table1X, 290);
	
	renderSpecial(character1_, 415, 495);
	renderSpecial(character2_, 415 + table2X - table1X, 495);

	SDL_RenderPresent(ren_);
}
void
View::renderText(const SDL_Color& color, SDL_Rect& destination, const std::string& text)
{
	if (font_ == nullptr) {
		throw std::runtime_error("No font declared");
	}
	SDL_Surface* surf = TTF_RenderText_Blended(font_, text.c_str(), color);
	SDL_Texture* score = SDL_CreateTextureFromSurface(ren_, surf);
	SDL_FreeSurface(surf);
	SDL_QueryTexture(score, NULL, NULL, &destination.w, &destination.h);
	SDL_RenderCopy(ren_, score, NULL, &destination);
	SDL_DestroyTexture(score);
}
