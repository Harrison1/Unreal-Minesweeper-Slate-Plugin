// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "Slate/SObjectWidget.h"

class SMineSweep : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SMineSweep) {}
	SLATE_END_ARGS()

	/** initial construct */
	void Construct(const FArguments& InArgs);

	/** simpole values to track game board data */
	int32 TotalRows = 5;
	int32 TotalColumns = 10;
	int32 TotalBombs = 10;

	/** Arrays to keep track of bomb locations and status of cells (whether they've been clicked or not)*/
	TArray<FVector2D> BombLocations;
	TArray<FVector2D> OpenCells;

	/** Assignable slate text elements */
	TSharedPtr<STextBlock> ButtonIndex;
	TSharedPtr<STextBlock> GameStateText;
	TSharedPtr<SErrorText> ErrorText;
	
	/** Editable Slate Elements */
	TSharedPtr<SEditableTextBox> Rows;
	TSharedPtr<SEditableTextBox> Columns;
	TSharedPtr<SEditableTextBox> Bombs;

	/** UnivormGridPanel Slate Elements for the game board */
	TSharedPtr<SUniformGridPanel> ButtonGrid = SNew(SUniformGridPanel);
	TSharedPtr<SUniformGridPanel> BombGrid = SNew(SUniformGridPanel);

	/** Array of Buttons widgets to we can add and remove them as needed */
	TArray<TSharedRef<SWidget>> ButtonGridWidgets;

	/** Generate new game */
	FReply GenerateNewGame();

	/** Generate game over grid */
	void GenerateGameOverGrid();

	/** 
	* Determine what happens when a player clicks a cell
	* 
	* @param FVector2D coordinates of where the player clicked
	* 
	* @returns FReply
	*/
	FReply GridCellClick(FVector2D Location);

	/** Reusable slate elements */
	TSharedRef<SBox> GenerateGridButton(FVector2D GridXY);
	TSharedRef<SBox> BombIconBox();
	TSharedRef<SBox> NumberIconBox(int32 NumHits);

	/*
	 * Randomly generate mine locations on grid.
	 *
	 * @param	NewBombTotal total of bombs.
	 * @param	GridSize size of the grid
	 */
	void GenerateBombLocations(int32 NewBombTotal, FVector2D GridSize);

	/*
	 * Get all of cell's neighbors
	 *
	 * @param	Cell xy
	 */
	TArray<FVector2D> GenerateNeighborLocations(int32 X, int32 Y);

	/*
	 * Determine what to do if the user clicks a tile without a bomb. 
	 * If the user clicks a button not touching any bombs this function will recursively run
	 * until it hits numbered tiles.
	 *
	 * @param	XY of the cell being operated on.
	 */
	void RecurseCellOperation(FVector2D Coords);

	/*
	 * Return color for number
	 *
	 * @param	Num simple int32 representing how many mines the cell is touching.
	 * @returns FLinearColor to paint the slate text's ColorAndOpacity.
	 */
	FLinearColor NumberColor(int32 Num);
};
