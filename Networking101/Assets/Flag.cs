using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class Flag : NetworkBehaviour {

	enum State
	{
		Available,
		Possessed
	};

	[SyncVar]
	State m_state;

	// Use this for initialization
	void Start () {
        //Vector3 spawnPoint;
        //ObjectSpawner.RandomPoint(this.transform.position, 10.0f, out spawnPoint);
        //this.transform.position = spawnPoint;
        //GetComponent<MeshRenderer> ().enabled = false;
        m_state = State.Available;

    }

    [ClientRpc]
    public void RpcPickUpFlag(GameObject player)
    {
        AttachFlagToGameObject(player);
    }

    public void AttachFlagToGameObject(GameObject obj)
    {
        this.transform.parent = obj.transform;
    }

    void OnTriggerEnter(Collider other)
    {
        if(m_state == State.Possessed || !isServer || other.tag != "Player")
        {
            return;
        }

        m_state = State.Possessed;
        AttachFlagToGameObject(other.gameObject);
        RpcPickUpFlag(other.gameObject);
    }

    // Update is called once per frame
    void Update () {
		
	}
}
