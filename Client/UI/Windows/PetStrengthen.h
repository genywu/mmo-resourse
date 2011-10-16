#pragma  once

struct PetStrenthenType 
{
	long typeID;
	std::string StrenthenText;
};

class PetStrenthen
{
public:
	PetStrenthen(void) {}
	~PetStrenthen(void) {}

	void SetType(long type) { StrenthenType = type; }

	long GetType(void) { return StrenthenType; }

	bool  OnOpen(const CEGUI::EventArgs& e);

	bool  OnClose(const CEGUI::EventArgs& e);   
private:
	long StrenthenType;
};