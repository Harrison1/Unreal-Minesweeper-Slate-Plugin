// Fill out your copyright notice in the Description page of Project Settings.

#include "SMineSweep.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Internationalization/Text.h"

#define LOCTEXT_NAMESPACE "SMineSweep"

void SMineSweep::Construct(const FArguments& InArgs)
{
	GenerateBombLocations(TotalBombs, FVector2D(TotalColumns, TotalRows));

	for (int32 i = 0; i < TotalRows; i++) {
		for (int32 j = 0; j < TotalColumns; j++)
		{

			TSharedPtr<SBox> GridButton = GenerateGridButton(FVector2D(j, i));

			ButtonGridWidgets.AddUnique(GridButton.ToSharedRef());

			ButtonGrid->AddSlot(j, i)
				[
					GridButton.ToSharedRef()
				];
		}
	}

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Padding(5.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Rows"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(100.0f)
					.HeightOverride(14.0f)
					[
						SAssignNew(Rows, SEditableTextBox)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						.HintText(LOCTEXT("Rows", "Rows"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Padding(5.0f)
					[
						SNew(STextBlock).Text(FText::FromString("Columns"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(100.0f)
					.HeightOverride(14.0f)
					[
						SAssignNew(Columns, SEditableTextBox)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						.HintText(LOCTEXT("Columns", "Columns"))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.VAlign(VAlign_Center)
					.Padding(5.0f)
					[
						SNew(STextBlock).Text(FText::FromString("Bombs"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					]
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(100.0f)
					.HeightOverride(14.0f)
					[
						SAssignNew(Bombs, SEditableTextBox)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						.HintText(LOCTEXT("Bombs", "Bombs"))
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				[
					SNew(SButton)
					.OnClicked(this, &SMineSweep::GenerateNewGame)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Generate New Game"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				[
					SAssignNew(ErrorText, SErrorText)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					ButtonGrid.ToSharedRef()
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				[
					SAssignNew(GameStateText, STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
					.Text(FText::FromString(""))
					.ColorAndOpacity(FLinearColor::Red)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HAlign(HAlign_Left)
				[
					SAssignNew(ButtonIndex, STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 18))
				]
			]
		];

	Rows->SetText(FText::AsNumber(TotalRows));
	Columns->SetText(FText::AsNumber(TotalColumns));
	Bombs->SetText(FText::AsNumber(TotalBombs));
}

FReply SMineSweep::GridCellClick(FVector2D Location)
{
	if (BombLocations.Contains(Location))
	{
		GenerateGameOverGrid();
	}
	else
	{
		RecurseCellOperation(Location);
	}

	return FReply::Handled();
}

FReply SMineSweep::GenerateNewGame()
{
	FText Height = Rows->GetText();
	FText Width = Columns->GetText();
	FText Mines = Bombs->GetText();

	if (!Width.IsNumeric() || !Height.IsNumeric() || !Mines.IsNumeric())
	{

		ErrorText->SetError(FText::FromString("Invalid input: Please enter numbers between 1 and 30."));
		return FReply::Handled();
	}

	int32 RowInt = FMath::Floor(FMath::Clamp(FCString::Atof(*Height.ToString()), 1.0f, 30.0f));
	int32 ColInt = FMath::Floor(FMath::Clamp(FCString::Atof(*Width.ToString()), 1.0f, 30.0f));
	int32 NewMines = FMath::Floor(FMath::Clamp(FCString::Atof(*Mines.ToString()), 1.0f, 30.0f));

	if (NewMines >= (ColInt * RowInt))
	{
		ErrorText->SetError(FText::FromString("Invalid input: Too many bombs. Reduce the bombs."));
		return FReply::Handled();
	}

	Rows->SetText(FText::AsNumber(RowInt));
	Columns->SetText(FText::AsNumber(ColInt));
	Bombs->SetText(FText::AsNumber(NewMines));

	TotalRows = RowInt;
	TotalColumns = ColInt;
	TotalBombs = NewMines;

	ButtonGrid->ClearChildren();
	ButtonGridWidgets.Empty();
	BombLocations.Empty();
	OpenCells.Empty();

	GenerateBombLocations(NewMines, FVector2D(ColInt, RowInt));

	for (int32 NewRow = 0; NewRow < RowInt; NewRow++)
	{
		for (int32 NewCol = 0; NewCol < ColInt; NewCol++)
		{

			TSharedPtr<SBox> GridButton = GenerateGridButton(FVector2D(NewCol, NewRow));

			ButtonGridWidgets.AddUnique(GridButton.ToSharedRef());

			ButtonGrid->AddSlot(NewCol, NewRow)
				[
					GridButton.ToSharedRef()
				];
		}
	}

	ErrorText->SetError(FText::FromString(""));
	GameStateText->SetText(FText::FromString(""));
	return FReply::Handled();
}

TSharedRef<SBox> SMineSweep::GenerateGridButton(FVector2D GridXY)
{
	TSharedRef<SBox> GridButton = SNew(SBox)
		.WidthOverride(24.0f)
		.HeightOverride(24.0f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FColor(192, 192, 192, 255))
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton).OnClicked(this, &SMineSweep::GridCellClick, GridXY)
				]
			]
		];

	return GridButton;
}

void SMineSweep::GenerateBombLocations(int32 NewBombTotal, FVector2D GridSize)
{
	while (BombLocations.Num() < NewBombTotal)
	{
		int32 MaxX = GridSize.X - 1;
		int32 MaxY = GridSize.Y - 1;

		float BLX = FMath::RandRange(0, MaxX);
		float BLY = FMath::RandRange(0, MaxY);

		FVector2D Local2D = FVector2D(BLX, BLY);
		BombLocations.AddUnique(Local2D);
	}
}

TArray<FVector2D> SMineSweep::GenerateNeighborLocations(int32 X, int32 Y)
{
	TArray<FVector2D> Neighbors;

	float XMinusOne = FMath::Clamp(X - 1.0f, 0.0f, (TotalColumns - 1.0f));
	float XPlusOne = FMath::Clamp(X + 1.0f, 0.0f, (TotalColumns - 1.0f));

	float YMinusOne = FMath::Clamp(Y - 1.0f, 0.0f, (TotalRows - 1.0f));
	float YPlusOne = FMath::Clamp(Y + 1.0f, 0.0f, (TotalRows - 1.0f));

	FVector2D one = FVector2D(XMinusOne, Y);
	FVector2D two = FVector2D(XPlusOne, Y);

	FVector2D three = FVector2D(X, YMinusOne);
	FVector2D four = FVector2D(X, YPlusOne);

	FVector2D five = FVector2D(XMinusOne, YMinusOne);
	FVector2D six = FVector2D(XMinusOne, YPlusOne);

	FVector2D seven = FVector2D(XPlusOne, YMinusOne);
	FVector2D eight = FVector2D(XPlusOne, YPlusOne);

	Neighbors.AddUnique(one);
	Neighbors.AddUnique(two);
	Neighbors.AddUnique(three);
	Neighbors.AddUnique(four);
	Neighbors.AddUnique(five);
	Neighbors.AddUnique(six);
	Neighbors.AddUnique(seven);
	Neighbors.AddUnique(eight);

	return Neighbors;
}

TSharedRef<SBox> SMineSweep::BombIconBox()
{
	TSharedRef<SBox> BombBox = SNew(SBox)
		.WidthOverride(24.0f)
		.HeightOverride(24.0f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FColor(192, 192, 192, 255))
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					.Text(FText::FromString("*"))
					.ColorAndOpacity(FLinearColor(FColor::Red))
				]
			]
		];

	return BombBox;
}

TSharedRef<SBox> SMineSweep::NumberIconBox(int32 NumHits)
{
	FText Indicator = NumHits > 0 ? FText::AsNumber(NumHits) : FText::FromString("");

	TSharedRef<SBox> NumberBox = SNew(SBox)
		.WidthOverride(24.0f)
		.HeightOverride(24.0f)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FColor(192, 192, 192, 255))
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
					.Text(Indicator)
					.ColorAndOpacity(NumberColor(NumHits))
				]
			]
		];

	return NumberBox;
}

void SMineSweep::GenerateGameOverGrid()
{
	ButtonGrid->ClearChildren();
	ButtonGridWidgets.Empty();
	GameStateText->SetText(FText::FromString("Game Over"));
	GameStateText->SetColorAndOpacity(FLinearColor::Red);

	for (int32 NewRow = 0; NewRow < TotalRows; NewRow++)
	{
		for (int32 NewCol = 0; NewCol < TotalColumns; NewCol++)
		{
			TSharedPtr<SBox> Tile;
			FVector2D CurrentCoords = FVector2D(NewCol, NewRow);

			if (BombLocations.Contains(CurrentCoords))
			{
				Tile = BombIconBox();
			}
			else
			{
				TArray<FVector2D> CellNeighbors = GenerateNeighborLocations(NewCol, NewRow);
				TArray<FVector2D> Hits;

				for (int32 j = 0; j < 8; j++)
				{
					if (CellNeighbors.IsValidIndex(j) && BombLocations.Contains(CellNeighbors[j]))
					{
						Hits.AddUnique(CellNeighbors[j]);
					}
				}

				Tile = NumberIconBox(Hits.Num());
			}

			ButtonGrid->AddSlot(NewCol, NewRow)
				[
					Tile.ToSharedRef()
				];
		}
	}
}

void SMineSweep::RecurseCellOperation(FVector2D Coords)
{
	int32 AIndex = (Coords.Y * TotalColumns + Coords.X);

	ButtonGrid->RemoveSlot(ButtonGridWidgets[AIndex]);
	OpenCells.AddUnique(FVector2D(Coords.X, Coords.Y));

	if (OpenCells.Num() == (TotalRows * TotalColumns - TotalBombs))
	{
		GenerateGameOverGrid();
		GameStateText->SetText(FText::FromString("You Win"));
		GameStateText->SetColorAndOpacity(FLinearColor::Green);
		return;
	}

	TArray<FVector2D> CellNeighbors = GenerateNeighborLocations(Coords.X, Coords.Y);
	TArray<FVector2D> TouchingBombs;

	for (int32 j = 0; j < CellNeighbors.Num(); j++)
	{
		if (BombLocations.Contains(CellNeighbors[j]))
		{
			TouchingBombs.AddUnique(CellNeighbors[j]);
		}
	}

	if (TouchingBombs.Num() == 0)
	{
		ButtonGrid->AddSlot(Coords.X, Coords.Y)
			[
				NumberIconBox(TouchingBombs.Num())
			];

		for (int32 n = 0; n < CellNeighbors.Num(); n++)
		{
			if (!OpenCells.Contains(CellNeighbors[n]))
			{
				RecurseCellOperation(CellNeighbors[n]);
			}
		}
	}
	else
	{
		ButtonGrid->AddSlot(Coords.X, Coords.Y)
			[
				NumberIconBox(TouchingBombs.Num())
			];
	}
}

FLinearColor SMineSweep::NumberColor(int32 Num)
{
	switch (Num) {
		case (1):
			return FLinearColor(FColor::Cyan);
		case (2):
			return FLinearColor::Green;
		case (3):
			return FLinearColor(FColor::White);
		case (4):
			return FLinearColor(FColor::Magenta);
		case (5):
			return FLinearColor(FColor::Orange);
		case (6):
			return FLinearColor(FColor::Turquoise);
		case (7):
			return FLinearColor(FColor::Yellow);
		case (8):
			return FLinearColor(FColor::Silver);
		default:
			return FLinearColor::Green;
	}
}
