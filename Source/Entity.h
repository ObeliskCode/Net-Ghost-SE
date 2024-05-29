#ifndef ENTITY_H
#define ENTITY_H

class Entity {
	public:
		Entity(unsigned int ID) { m_id = ID; };

		/// <summary>
		///  these are the 32 bits we will limit ourselves to. some combo of C style bitfield and enums...
		///	 we need the entity class to atleast be smaller than a ptr, (64 bits)
		/// </summary>
		unsigned int m_id : 16; // steal 16 bits from 32 bit int instead of changing to short...
		unsigned int transform_flag : 1 = 0;
		unsigned int model_flag : 1 = 0;
		unsigned int skmodel_flag : 1 = 0;
		unsigned int physbody_flag : 1 = 0;
		unsigned int phystransform_flag : 1 = 0;
		unsigned int animator_flag : 1 = 0;
		unsigned int shader_flag : 1 = 0;
		unsigned int camera_flag : 1 = 0;
		unsigned int stencil_flag : 1 = 0;
		unsigned int visible_flag : 1 = 1;
		unsigned int dynamic_flag : 1 = 0;
		unsigned int pickup_flag : 1 = 0;
		unsigned int surface_flag : 1 = 0;
		unsigned int light_flag : 1 = 0;
		unsigned char m_camera : 2 = 0; // 0 = cam1, 1 = cam2, 2 = cam3, 3 = camx

	private:
};

#endif
