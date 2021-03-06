#include "Logic.hh"

Shape
rotate(const Shape& shape)
{
	Shape rotated;
	for (size_t i = 0; i < shape.at(0).size(); ++i) {
		std::vector<Color> tmp;
		for (size_t j = 0; j < shape.size(); ++j) {
			tmp.push_back(shape[j][i]);
		}
		std::reverse(tmp.begin(), tmp.end());
		rotated.push_back(tmp);
	}
	return rotated;
}

Logic::Logic()
{
	currentShape_ = std::make_unique<Tetromino>();
	nextShape_ = std::make_unique<Tetromino>();
	clear();
}

Logic::~Logic()
{
}

void
LogicInterface::clear()
{
	for (size_t i = 0; i < landedTable_.size(); i++) {
		for (size_t j = 0; j < landedTable_.at(i).size(); j++) {
			landedTable_[i][j] = Color::none;
		}
	}
}

LogicInterface::TetrisTable
LogicInterface::getTableWithShape()
{
	if (!currentShape_) return landedTable_;
	if (!canMoveTo(currentShape_->shape, currentShape_->topLeft)) return landedTable_;
	TetrisTable fullTable = landedTable_;
	for (size_t i = 0; i < currentShape_->shape.size(); i++) {
		for (size_t j = 0; j < currentShape_->shape.at(i).size(); j++) {
			if (currentShape_->shape.at(i).at(j) == Color::none) continue;
			fullTable.at(i + currentShape_->topLeft.second).at(j + currentShape_->topLeft.first)
				= currentShape_->shape.at(i).at(j);
		}
	}
	return fullTable;
}

bool
LogicInterface::canMoveTo(const Shape& shape, const Position& nextPos)
{
	for (size_t i = 0; i < shape.size(); i++) {
		for (size_t j = 0; j < shape.at(i).size(); j++) {
			if (shape.at(i).at(j) == Color::none) continue;
			unsigned nextX = i + nextPos.second;
			unsigned nextY = j + nextPos.first;
			if (nextX >= landedTable_.size() || nextY >= landedTable_.at(j).size()) return false;
			if (landedTable_.at(nextX).at(nextY) != Color::none) {
				return false;
			}
		}
	}
	return true;
}

void
Logic::cleanLine(size_t line)
{
	for (size_t i = line; i > 0; i--) {
		landedTable_.at(i).swap(landedTable_.at(i - 1));
	}
	for (size_t i = 0; i < landedTable_.at(0).size(); i++) {
		if (landedTable_.at(0).at(i) == Color::magic) currentMana_++;
		landedTable_.at(0).at(i) = Color::none;
	}
}

void
Logic::cleanFullLines()
{
	for (size_t i = 0; i < landedTable_.size(); i++) {
		bool complete = true;
		for (size_t j = 0; j < landedTable_.at(i).size(); j++) {
			if (landedTable_.at(i).at(j) == Color::none) {
				complete = false;
				break;
			}
		}
		if (complete) {
			if (enemy_ != nullptr) enemy_->enemyClearedLine();
			cleanLine(i);
		}
	}
}

void
Logic::landCurrent()
{
	landedTable_ = getTableWithShape();
	resetCurrent();
	cleanFullLines();
	while(linesToAdd_) {
		linesToAdd_--;
		addLine();
	}
}

void
Logic::resetCurrent()
{
	std::swap(nextShape_, currentShape_);
	nextShape_ = std::make_unique<Tetromino>();
}

void
Logic::update()
{
	Position nextPos_ = currentShape_->topLeft;
	nextPos_.second += 1;
	if (canMoveTo(currentShape_->shape, nextPos_)) currentShape_->topLeft = nextPos_;
	else landCurrent();
}

void
Logic::move(unsigned x, unsigned y)
{
	Position nextPos_ = currentShape_->topLeft;
	nextPos_.first += x;
	nextPos_.second += y;
	if (canMoveTo(currentShape_->shape, nextPos_)) currentShape_->topLeft = nextPos_;
}

bool
Logic::finished() {
	gameFailed_ = (!canMoveTo(currentShape_->shape, currentShape_->topLeft));
	return gameFailed_;
}

void
Logic::clearStats()
{
	clearMana();
}

void
Logic::newGame()
{
	gamesWon_ += gameFailed_ ? 0 : 1;
	gameFailed_ = false;
	currentShape_ = std::make_unique<Tetromino>();
	nextShape_ = std::make_unique<Tetromino>();
	clearStats();
	clear();
}

void
Logic::rotate()
{
	auto newShape = ::rotate(currentShape_->shape);
	if (canMoveTo(newShape, currentShape_->topLeft)) {
		std::swap(newShape, currentShape_->shape);
	}
}

void
Logic::addLine()
{
	for (size_t i = 0; i < landedTable_.size() - 1; i++) {
		landedTable_.at(i + 1).swap(landedTable_.at(i));
	}

	const size_t lastLine = landedTable_.size() - 1;
	const size_t skippedPos = (rand() % landedTable_.at(lastLine).size());
	for (size_t i = 0; i < landedTable_.at(lastLine).size(); i++) {
		if (i == skippedPos) landedTable_.at(lastLine).at(i) = Color::none;
		else landedTable_.at(lastLine).at(i) = Color::grey;
	}
}

void
Logic::removeLine()
{
	int last = landedTable_.size() - 1;
	for (size_t i = 0; i < landedTable_.at(last).size(); i++) {
		landedTable_.at(last).at(i) = Color::none;
	}
	for (size_t i = landedTable_.size() - 1; i > 0; i--) {
		landedTable_.at(i - 1).swap(landedTable_.at(i));
	}
}

void
Logic::removeTopLines(size_t lines)
{
	size_t firstNonEmpty = 0;
	for (firstNonEmpty = 0; firstNonEmpty < landedTable_.size(); firstNonEmpty++) {
		if (std::find_if_not(landedTable_.at(firstNonEmpty).begin(), landedTable_.at(firstNonEmpty).end(), [](auto i) { return (i == Color::none); }) != landedTable_.at(firstNonEmpty).end()) break;
	}
	for (size_t i = 0; i < lines; i++) {
		if (firstNonEmpty >= landedTable_.size()) return;
		for (size_t i = 0; i < landedTable_.at(firstNonEmpty).size(); i++) {
			landedTable_.at(firstNonEmpty).at(i) = Color::none;
		}
		firstNonEmpty++;
	}
}

bool
LogicInterface::pointIsEmpty(unsigned x, unsigned y)
{
	if (x >= landedTable_.size() || y >= landedTable_.at(0).size()) return false;
	return (landedTable_.at(x).at(y) == Color::none);
}
