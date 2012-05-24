
//store hand history to idenify the complex gesture


#pragma once
class hand_h
{
public:
	int MAX_HANDLIST;
	XnPoint3D current;
	XnPoint3D prev;

	hand_h();
	~hand_h();

	//storing
	void storeHand(XnPoint3D p);
	void clearHandList();

	//get info
	float gettranslateX();
	float gettranslateY();
	float gettranslateZ();
};