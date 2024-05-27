#ifndef ENTITY_H
#define ENTITY_H

class Entity {
	public:
		Entity(unsigned int ID) { m_id = ID; };

		//void setType(std::string type);
		//std::string getType();
		/*
		bool m_visible;
		bool m_surface;
		*/

		/// <summary>
		///  these are the 32 bits we will limit ourselves to. some combo of C style bitfield and an enum type...
		///	 we need the entity class to atleast be smaller than a ptr, (64 bits)
		/// </summary>
		unsigned int m_id : 16; // steal 16 bits from 32 bit int instead of changing to short (16 bits)?
		unsigned int transform_flag : 1 = 0;
		unsigned int model_flag : 1 = 0;
		unsigned int skmodel_flag : 1 = 0;
		unsigned int physbody_flag : 1 = 0;
		unsigned int phystransform_flag : 1 = 0;
		unsigned int animator_flag : 1 = 0;
		unsigned int wire_flag : 1 = 0;
		unsigned int shader_flag : 1 = 0;
		unsigned int camera_flag : 1 = 0;
		unsigned int stencil_flag : 1 = 0;
		unsigned int visible_flag : 1 = 1;
		unsigned int dynamic_flag : 1 = 0;
		char m_enumtype : 4 = 0; // 15 possible types with 4 bits including NONE

		/*
		std::string m_type; // put within m_flag or m_flag2?
		*/

	private:
};

#endif
