using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class PowerUp : NetworkBehaviour
{

    CTFGameManager CTF;

    // Use this for initialization
    void Start () {
        CTF = GameObject.FindGameObjectWithTag("GameManger").GetComponent<CTFGameManager>();
	}
	
	// Update is called once per frame
	void Update () {

    }

    [ClientRpc]
    public void RpcPickupPower()
    {
        RpcPickupPower();
    }
    

    public void CmdRpcPickupPower()
    {
        Destroy(gameObject);
    }

    [ClientRpc]
    public void RpcPowerUp(GameObject player)
    {
        PowerUpFunc(player);
    }

    public void PowerUpFunc(GameObject obj)
    {
        obj.GetComponent<PlayerController>().isPoweredUp = true;
        if (obj.GetComponent<PlayerController>().hasFlag)
        {
            //Heal 
            obj.GetComponent<Health>().currentHealth += 50;
            Debug.Log("HAS FLAG");
        }
        else
        {
            obj.GetComponent<PlayerController>().m_linearSpeed = 3.0f;
            Debug.Log("NO FLAG");
        }
    }

    void OnTriggerEnter(Collider other)
    {

        if (!isServer || other.tag != "Player")
        {
            return;
        }

        CTF.powerUpCount--;

        PowerUpFunc(other.gameObject);
        RpcPowerUp(other.gameObject);

        CmdRpcPickupPower();
        RpcPickupPower();
    }
}
