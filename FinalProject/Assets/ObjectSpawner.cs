using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.AI;
using UnityEngine.Networking;

public class ObjectSpawner : NetworkBehaviour {

    public GameObject m_playerPrefab = null;
    public GameObject m_flagPrefab = null;
    public Text m_timerText = null;

    public float m_range = 10.0f;

    public static bool RandomPoint(Vector3 center, float range, out Vector3 result)
    {
        for (int i = 0; i < 30; i++)
        {
            Vector2 randPoint = Random.insideUnitCircle;
            Vector3 randomPoint = center + new Vector3(randPoint.x,randPoint.y,center.z) * range;
            NavMeshHit hit;
            if (NavMesh.SamplePosition(randomPoint, out hit, 1.0f, NavMesh.AllAreas))
            {
                result = hit.position;
                return true;
            }
        }
        result = Vector3.zero;
        return false;
    }

    static public bool SpawnObject(GameObject objectToSpawn, float range)
    {
        Vector3 spawnPoint;
        if (RandomPoint(new Vector3(0.0f, 0.0f, 0.0f), range, out spawnPoint))
        {
            Quaternion rotation = objectToSpawn.transform.rotation;
            GameObject clone = (GameObject)Instantiate(objectToSpawn, spawnPoint, rotation);
            return true;
        }
        
        Debug.Log("Could not find point to spawn");
        return false;
    }

    // Use this for initialization
    void Start ()
    {
        //SpawnObject(m_playerPrefab);
			//SpawnObject (m_flagPrefab, m_range);
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
