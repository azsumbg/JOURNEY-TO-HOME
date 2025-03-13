#pragma once

#ifdef SPACE_EXPORTS 
#define SPACE_API __declspec(dllexport)
#else
#define SPACE_API __declspec(dllimport)
#endif

#include <random>

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };
constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr unsigned char DLL_Ok{ 0b00000000 };
constexpr unsigned char DLL_Error{ 0b00000001 };

constexpr uint16_t no_type{ 0b0000000000000000 };

constexpr uint16_t type_field1{ 0b0000000100000000 };
constexpr uint16_t type_field2{ 0b0000001000000000 };
constexpr uint16_t type_field3{ 0b0000010000000000 };
constexpr uint16_t type_explosion{ 0b0000100000000000 };

constexpr uint16_t object_meteor1{ 0b0000000000000001 };
constexpr uint16_t object_meteor2{ 0b0000000000000010 };
constexpr uint16_t object_meteor3{ 0b0000000000000100 };
constexpr uint16_t object_meteor4{ 0b0000000000001000 };
constexpr uint16_t object_meteor5{ 0b0000000000010000 };
constexpr uint16_t object_ship{ 0b0000000000100000 };
constexpr uint16_t object_earth{ 0b0000000001000000 };
constexpr uint16_t object_bullet{ 0b0000000010000000 };

enum class dirs { up = 0, down = 1, left = 2, right = 3, stop = 4 };

struct FPOINT
{
	float x{ 0 };
	float y{ 0 };
};

namespace dll
{
	class SPACE_API RandIt
	{
		private:
			std::random_device rd{};
			std::seed_seq* sq{ nullptr };
			std::mt19937* twister{ nullptr };

		public: 
			RandIt();
			~RandIt();

			int operator() (int min, int max);
	};

	class SPACE_API PROTON
	{
		protected:
			float width{ 0 };
			float height{ 0 };

		public:
			FPOINT start{};
			FPOINT end{};
			FPOINT center{};
			float Xradius{ 0 };
			float Yradius{ 0 };

			PROTON(float _sx = 0, float _sy = 0, float _width = 1.0f, float _height = 1.0f);
			virtual ~PROTON() {};

			void SetEdges();
			
			float GetWidth() const;
			float GetHeight() const;

			void SetWidth(float _new_width);
			void SetHeight(float _new_height);

			void NewDims(float _new_width, float _new_height);
	};

	class SPACE_API BASE :public PROTON
	{
		protected:
			uint16_t type = no_type;

			int max_frames = 0;
			int frame = 0;
			int frame_delay = 0;
			 
			float move_sx = 0;
			float move_sy = 0;
			float move_ex = 0;
			float move_ey = 0;
			float slope = 0;
			float intercept = 0;

			bool hor_line = false;
			bool vert_line = false;

			float speed = 0;

			void SetPath(float whereX, float whereY);

		public:
			dirs dir = dirs::stop;
			
			BASE(uint16_t _what, float _sx, float _sy);
			virtual~BASE() {};

			int GetFrame();

			uint16_t GetType() const;

			virtual void Release() = 0;
			virtual bool Move(float gear) = 0;
			virtual bool Move(dirs move_dir, float gear) = 0;
	};

	class SPACE_API TYPES :public BASE
	{
		private:

			TYPES(uint16_t _what_, float _where_x, float _where_y); 

		public:
			friend SPACE_API BASE* ObjectFactory(uint16_t what_field, float where_x, float where_y, 
				float to_x, float to_y);

			void Release() override;
			virtual bool Move(float gear) override;
			virtual bool Move(dirs move_dir, float gear) override;
	};

	class SPACE_API ACTIVE_OBJECT :public BASE
	{
	private:

		ACTIVE_OBJECT(uint16_t _what_, float _where_x, float _where_y, float _met_to_x, float _met_to_y);

	public:
		friend SPACE_API BASE* ObjectFactory(uint16_t what_field, float where_x, float where_y,
			float to_x, float to_y);

		void Release() override;
		virtual bool Move(float gear) override;
		virtual bool Move(dirs move_dir, float gear) override;
	};

	typedef BASE* DLLObject;

	SPACE_API BASE* ObjectFactory(uint16_t what_object, float where_x, float where_y, float to_x, float to_y);
}