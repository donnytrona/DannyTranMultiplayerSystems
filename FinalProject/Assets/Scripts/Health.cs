using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Networking;
using System.Collections;

public class Health : NetworkBehaviour {

    public const int maxHealth = 100;
    public bool destroyOnDeath;
    bool isDead = false;

    [SyncVar(hook = "OnChangeHealth")]
    public int currentHealth = maxHealth;

    public RectTransform healthBar;

    private NetworkStartPosition[] spawnPoints;
    public GameObject m_flag = null;
    private GameObject flag;

    void Start ()
    {
        if (isLocalPlayer)
        {
            spawnPoints = FindObjectsOfType<NetworkStartPosition>();
        }
    }

    public void TakeDamage(int amount)
    {
        if (!isServer)
            return;
        
        currentHealth -= amount;
        if (currentHealth <= 0)
        {
            if (destroyOnDeath)
            {
                Destroy(gameObject);
            } 
            else
            {
                currentHealth = maxHealth;

                if(this.GetComponent<PlayerController>().hasFlag)
                {
                    flag = GameObject.FindGameObjectWithTag("Flag");
                    NetworkServer.Destroy(flag);

                    GameObject flagSpawn = Instantiate(m_flag, new Vector3(0, 3, 8.34f), new Quaternion());
                    NetworkServer.Spawn(flagSpawn);
                }

                // called on the Server, invoked on the Clients
                RpcRespawn();

                this.GetComponent<PlayerController>().hasFlag = false;
            }
        }
    }

    void OnChangeHealth (int currentHealth )
    {
        healthBar.sizeDelta = new Vector2(currentHealth , healthBar.sizeDelta.y);
    }

    [ClientRpc]
    public void RpcRespawn()
    {
        if (isLocalPlayer)
        {
            // Set the spawn point to origin as a default value
            Vector3 spawnPoint = Vector3.zero;

            // If there is a spawn point array and the array is not empty, pick one at random
            if (spawnPoints != null && spawnPoints.Length > 0)
            {
                spawnPoint = spawnPoints[GetComponent<PlayerController>().id].transform.position;
            }

            // Set the player’s position to the chosen spawn point
            transform.position = spawnPoint;
        }
    }
}
