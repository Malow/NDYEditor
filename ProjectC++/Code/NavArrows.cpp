#include "NavArrows.h"
#include "Graphics.h"


NavArrows::NavArrows( GraphicsEngine* gfx ) :
	zGraphics(gfx),
	zArrowScale(0.25f),
	zSpawnPos(0.0f, 0.0f, 0.0f)
{
	zNavArrowX = zGraphics->CreateMesh("Media/Models/Arrow/RedArrow.obj", Vector3(0, 0, 0));
	zNavArrowZ = zGraphics->CreateMesh("Media/Models/Arrow/BlueArrow.obj", Vector3(-7.5f * zArrowScale, 0, 7.5f * zArrowScale));
	zNavArrowSpawn = zGraphics->CreateMesh("Media/Models/Arrow/BlackArrow.obj", Vector3(0, 0, 0));

	zNavArrowX->RotateAxis(Vector3(0, 0, 1), 3.14159265359f * 0.5f); // Point down x-axis
	zNavArrowZ->RotateAxis(Vector3(1, 0, 0), -3.14159265359f * 0.5f); // Point down z-axis
	zNavArrowSpawn->RotateAxis(Vector3(0, 0, 1), 3.14159265359f * 0.5f); // Point down x-axis
	zNavArrowX->Scale(zArrowScale);
	zNavArrowZ->Scale(zArrowScale);
	zNavArrowSpawn->Scale(zArrowScale);
}


NavArrows::~NavArrows()
{
	zGraphics->DeleteMesh(zNavArrowX);
	zGraphics->DeleteMesh(zNavArrowZ);
	zGraphics->DeleteMesh(zNavArrowSpawn);
}

void NavArrows::Update( const Vector3& camPos, const Vector3& camDir )
{
	// Axis Arrows
	Vector3 camPosOffset = camPos;
	Vector3 camForwardoffset = camDir * 2.0f;
	Vector3 arrowOffset = Vector3(-7.5f * zArrowScale, 0, 7.5f * zArrowScale);
	Vector3 cameraOffset = Vector3(7.5f * zArrowScale, -1.0f, 0.0f); 
	zNavArrowX->SetPosition(camPosOffset + camForwardoffset + cameraOffset);
	zNavArrowZ->SetPosition(camPosOffset + camForwardoffset + cameraOffset + arrowOffset);

	// Spawn Arrow
	zNavArrowSpawn->SetPosition(camPosOffset + camForwardoffset + cameraOffset);
	Vector3 vecBetweenSpawnAndArrow = zSpawnPos - zNavArrowSpawn->GetPosition();
	vecBetweenSpawnAndArrow.Normalize();
	zNavArrowSpawn->ResetRotation();
	Vector3 vec = Vector3(0, -1, 0);
	Vector3 around = vec.GetCrossProduct(vecBetweenSpawnAndArrow);
	float angle = acos(vec.GetDotProduct(vecBetweenSpawnAndArrow) / (vec.GetLength() * vecBetweenSpawnAndArrow.GetLength()));
	zNavArrowSpawn->RotateAxis(around, angle);
}

void NavArrows::Show( bool )
{
	
}

void NavArrows::SetSpawnPos( const Vector3& spawnPos )
{
	zSpawnPos = spawnPos;
}

