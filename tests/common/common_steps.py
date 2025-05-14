import requests
from time import sleep

def wait_for_backend(backend_url, timeout=60):
    backend_health_check_url = f"{backend_url}/health"
    for _ in range(timeout):
        try:
            response = requests.get(backend_health_check_url)
            if response.status_code == 200:
                print(f"Backend is available at {backend_health_check_url}.")
                return
        except requests.ConnectionError:
            pass
        sleep(1)
    
    raise Exception(f"Backend is not available at {backend_health_check_url}")
