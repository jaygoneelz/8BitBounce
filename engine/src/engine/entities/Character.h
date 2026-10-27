#pragma once
namespace engine
{
	class game_object;

	/// \brief Base class defining the Characters in the game world
	class Character
	{
	public:
		/// \brief Constructor
		Character(ref<game_object> object);
		/// \brief Destructor
		~Character();

		/// \brief Methods
		// getter
		ref<game_object> object() const { return m_object; }

		// setter
		void set_object(ref<game_object> object) { m_object = object; }

		// virtual update method. Overriden by subclasses (e.g. Player)
		virtual void update(double dt) {}

	protected:
		// updating the acceleration of the Character's object
		void move(glm::vec3 accelerate, double dt);
		//updating the torque of the Character's object
		void turn(glm::vec3 torque, double dt);

		/// \brief Field
		// Character's game object
		ref<game_object> m_object;

	};
}
