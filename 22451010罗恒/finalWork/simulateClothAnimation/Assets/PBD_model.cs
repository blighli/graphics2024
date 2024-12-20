using UnityEngine;
using System.Collections;

public class PBD_model: MonoBehaviour {

	float 		t= 0.0333f;
	float		damping= 0.99f;
	float reflect = 0.5f;
	float friction = 0.5f;
	int[] 		E;
	float[] 	L;
	Vector3[] 	V;
	Vector3 gravity = new Vector3(0.0f, -9.8f, 0.0f);
	public GameObject sphere;
	public float r;
	// Use this for initialization
	void Start () 
	{
		Mesh mesh = GetComponent<MeshFilter> ().mesh;

		//Resize the mesh.
		int n=21;
		Vector3[] X  	= new Vector3[n*n];
		Vector2[] UV 	= new Vector2[n*n];
		int[] T	= new int[(n-1)*(n-1)*6];
		for(int j=0; j<n; j++)
		for(int i=0; i<n; i++)
		{
			X[j*n+i] =new Vector3(5-10.0f*i/(n-1), 0, 5-10.0f*j/(n-1));
			UV[j*n+i]=new Vector3(i/(n-1.0f), j/(n-1.0f));
		}
		int t=0;
		for(int j=0; j<n-1; j++)
		for(int i=0; i<n-1; i++)	
		{
			T[t*6+0]=j*n+i;
			T[t*6+1]=j*n+i+1;
			T[t*6+2]=(j+1)*n+i+1;
			T[t*6+3]=j*n+i;
			T[t*6+4]=(j+1)*n+i+1;
			T[t*6+5]=(j+1)*n+i;
			t++;
		}
		mesh.vertices	= X;
		mesh.triangles	= T;
		mesh.uv 		= UV;
		mesh.RecalculateNormals ();

		//Construct the original edge list
		int[] _E = new int[T.Length*2];
		for (int i=0; i<T.Length; i+=3) 
		{
			_E[i*2+0]=T[i+0];
			_E[i*2+1]=T[i+1];
			_E[i*2+2]=T[i+1];
			_E[i*2+3]=T[i+2];
			_E[i*2+4]=T[i+2];
			_E[i*2+5]=T[i+0];
		}
		//Reorder the original edge list
		for (int i=0; i<_E.Length; i+=2)
			if(_E[i] > _E[i + 1]) 
				Swap(ref _E[i], ref _E[i+1]);
		//Sort the original edge list using quicksort
		Quick_Sort (ref _E, 0, _E.Length/2-1);

		int e_number = 0;
		for (int i=0; i<_E.Length; i+=2)
			if (i == 0 || _E [i + 0] != _E [i - 2] || _E [i + 1] != _E [i - 1]) 
				e_number++;

		E = new int[e_number * 2];
		for (int i=0, e=0; i<_E.Length; i+=2)
			if (i == 0 || _E [i + 0] != _E [i - 2] || _E [i + 1] != _E [i - 1]) 
			{
				E[e*2+0]=_E [i + 0];
				E[e*2+1]=_E [i + 1];
				e++;
			}

		L = new float[E.Length/2];
		for (int e=0; e<E.Length/2; e++) 
		{
			int i = E[e*2+0];
			int j = E[e*2+1];
			L[e]=(X[i]-X[j]).magnitude;
		}

		V = new Vector3[X.Length];
		for (int i=0; i<X.Length; i++)
			V[i] = new Vector3 (0, 0, 0);
	}

	void Quick_Sort(ref int[] a, int l, int r)
	{
		int j;
		if(l<r)
		{
			j=Quick_Sort_Partition(ref a, l, r);
			Quick_Sort (ref a, l, j-1);
			Quick_Sort (ref a, j+1, r);
		}
	}

	int  Quick_Sort_Partition(ref int[] a, int l, int r)
	{
		int pivot_0, pivot_1, i, j;
		pivot_0 = a [l * 2 + 0];
		pivot_1 = a [l * 2 + 1];
		i = l;
		j = r + 1;
		while (true) 
		{
			do ++i; while( i<=r && (a[i*2]<pivot_0 || a[i*2]==pivot_0 && a[i*2+1]<=pivot_1));
			do --j; while(  a[j*2]>pivot_0 || a[j*2]==pivot_0 && a[j*2+1]> pivot_1);
			if(i>=j)	break;
			Swap(ref a[i*2], ref a[j*2]);
			Swap(ref a[i*2+1], ref a[j*2+1]);
		}
		Swap (ref a [l * 2 + 0], ref a [j * 2 + 0]);
		Swap (ref a [l * 2 + 1], ref a [j * 2 + 1]);
		return j;
	}

	void Swap(ref int a, ref int b)
	{
		int temp = a;
		a = b;
		b = temp;
	}

	void Strain_Limiting()
	{
		Mesh mesh = GetComponent<MeshFilter> ().mesh;
		Vector3[] vertices = mesh.vertices;
		Vector3[] vertices_new = new Vector3[vertices.Length];
		int[] n = new int[vertices.Length];
		for(int i = 0; i < vertices.Length; i++)
		{
			vertices_new[i] = new Vector3(0.0f, 0.0f, 0.0f);
			n[i] = 0;
		}
		
		for(int e = 0; e < L.Length; e++)//注意是消重的
		{
			int a = E[e * 2 + 0];
			int b = E[e * 2 + 1];
			Vector3 a_b = vertices[a] - vertices[b];
			float halfDistance = (a_b.magnitude - L[e])*0.5f;
			Vector3 pointMove = halfDistance * a_b.normalized;

			vertices_new[a] = vertices_new[a] + vertices[a] - pointMove;
			vertices_new[b] = vertices_new[b] + vertices[b] + pointMove;
			n[a]++;
			n[b]++;
			
		}
		for(int i = 0; i < vertices.Length; i++)
		{
			if (i == 0 || i == 20) continue;
			V[i] = V[i] + ((vertices_new[i] + 0.2f * vertices[i]) / (n[i] + 0.2f) - vertices[i]) / t;
			vertices[i] = (vertices_new[i] + 0.2f * vertices[i]) / (n[i] + 0.2f);
		}
		//Apply PBD here.
		//...
		mesh.vertices = vertices;
	}

	void Collision_Handling()
	{
		Mesh mesh = GetComponent<MeshFilter> ().mesh;
		Vector3[] X = mesh.vertices;
		Vector3 spherePosition = sphere.transform.position;
		for(int i = 0; i < X.Length; i++)
		{
			if (i == 0 || i == 20) continue;
			if ((X[i] - spherePosition).magnitude > r)
			{
				continue;
			}

			//发生碰撞，得到碰撞点
			Vector3 collosionPoint = r * (X[i] - spherePosition).normalized + spherePosition;
			Vector3 normal = (collosionPoint - spherePosition).normalized;
			float jud = Vector3.Dot(V[i], normal);
			/*V[i] = V[i]+ (collosionPoint - X[i]) / t;
            X[i] = collosionPoint;*/
			
			Vector3 v_N = jud * normal;
			Vector3 v_T = V[i] - v_N;
			//作业这里的意思是碰撞以后，位移到球体表面
			v_N = v_N + (collosionPoint - X[i]) / t;
			X[i] = collosionPoint;
			V[i] = v_N + v_T;//忽略摩擦
		}
		//For every vertex, detect collision and apply impulse if needed.
		//...
		mesh.vertices = X;
	}

	// Update is called once per frame
	void Update () 
	{
		Mesh mesh = GetComponent<MeshFilter> ().mesh;
		Vector3[] X = mesh.vertices;

		for(int i=0; i<X.Length; i++)
		{
			if(i==0 || i==20)	continue;
			V[i] = V[i] + gravity * t;
			V[i] *= damping;
			X[i] = X[i] + V[i] * t;
			//Initial Setup
			//...
		}
		mesh.vertices = X;

		for(int l=0; l<32; l++)
			Strain_Limiting ();

		Collision_Handling ();

		mesh.RecalculateNormals ();

	}


}

