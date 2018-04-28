using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class Flag : NetworkBehaviour {

    [SyncVar]
    public bool frozen = false;

    public ParticleSystem PE;

    private GameObject currentAttached;

    enum State
    {
        Available,
        Possessed
    };

    [SyncVar]
    State m_state;

    // Use this for initialization
    void Start()
    {
        m_state = State.Available;

    }

    [ClientRpc]
    public void RpcPickUpFlag(GameObject player)
    {
        AttachFlagToGameObject(player);
    }

    public void AttachFlagToGameObject(GameObject obj)
    {
        PE.Stop();
        this.transform.parent = obj.transform;
    }

    void OnTriggerEnter(Collider other)
    {
        if(m_state == State.Possessed || !isServer || other.tag != "Player")
        {
            return;
        }

        m_state = State.Possessed;
        InvokeRepeating("AddToScore", 0, 1);
        other.gameObject.GetComponent<PlayerController>().hasFlag = true;
        currentAttached = other.gameObject;
        AttachFlagToGameObject(other.gameObject);
        RpcPickUpFlag(other.gameObject);
    }

    // Update is called once per frame
    void Update () {
        if (frozen)
            CancelInvoke();
    }

    void AddToScore()
    {
        currentAttached.GetComponent<Score>().m_score = currentAttached.GetComponent<Score>().m_score + 1;
    }
}
