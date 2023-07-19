#ifndef SceneIncluded
#define SceneIncluded



namespace D3D
{
	class Model;

	class Scene final
	{
	public:
		explicit Scene(const std::string& name);

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		const std::string& GetName() const { return m_Name; }

		void OnSceneLoad();

		void OnSceneUnload();

		void StartFrame();

		void Update();

		void FixedUpdate();

		void LateUpdate();

		void Render() const;

		void OngGUI() const;

		void Cleanup();

	private:
		std::string m_Name;

		std::vector<std::unique_ptr<Model>> m_pModels{};

		static unsigned int m_IdCounter;
	};
}

#endif // !SceneIncluded