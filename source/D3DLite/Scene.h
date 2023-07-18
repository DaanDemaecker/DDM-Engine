#ifndef SceneIncluded
#define SceneIncluded



namespace D3D
{
	class Scene final
	{
	public:
		explicit Scene(const std::string& name);

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;


		void OnSceneLoad();

		void OnSceneUnload();

		void StartFrame();

		void Update();

		void FixedUpdate();

		void LateUpdate();

		void Render() const;

		void OngGUI() const;

		void PostUpdate();

	private:


		std::string m_Name;

		static unsigned int m_IdCounter;
	};
}

#endif // !SceneIncluded