#ifndef MESHOBJECT_H_
#define MESHOBJECT_H_

/**
 * Base class for mesh objects.
 * This could be animating water, or the static sea bed.
 */
class MeshObject
{
	public:
		/**
		 * Constructor
		 * @param aName The name of the object's node within the scenegraph. This can be anything
		 */
		MeshObject(std::string aName);

		/**
		 * Get OSG geometry geode
		 */
		osg::Geode* get(){ return _node; }

		/**
		 * Turn culling on and off.
		 * Recreates the mesh with steep culling turned on or off.
		 * Steep faces are given an alpha of zero, and so do not appear.
		 * @param value enable or disable culling.
		 */
		void setCulling(bool value);

		/**
		 * Get culling state of mesh.
		 * @see setCulling
		 * @return true if culling is enabled
		 */
		bool getCulling(){ return _culling; };

		/**
		 * Set mesh to wireframe mode.
		 * The class will do its own dirty test, so this can be safely called every frame.
		 * @param value Set true to make mesh display as a wireframe
		 */
		virtual void setWireframe(bool value);

		/**
		 * Is wireframe
		 * @return true if the mesh is in wireframe mode.
		 */
		virtual bool getWireframe(){ return _wireframe; };

		/**
		 * Update this MeshObject
		 * Every MeshObject must have an update implemented.
		 */
		virtual void update();

		/**
		 * Data needs refreshing
		 * Rebuild the mesh if its data has changed.
		 */
		virtual void onRefreshData() = 0;

		/**
		 * Update timestep
		 * Animate this mesh.
		 * @param aTs timestep to skip to. Must be within range of animation frames.
		 */
		void setTimeStep( unsigned int aTs );

	protected:
		osg::StateSet* _stateset;
		osg::Geode* _node;
		osg::Geometry* _geom;
		class SWWReader* _sww;

		unsigned int _timestep;	/**< Current mesh animation frame */

	private:
		bool _wireframe, _dirtywireframe;
		bool _culling, _dirtyculling;
	    bool _dirtydata;	/**< Mesh data has changed if true */
};

#endif // MESHOBJECT_H_