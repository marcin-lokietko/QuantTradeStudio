import { Component } from '@angular/core';

@Component({
  selector: 'app-wallet-page',
  templateUrl: './wallet-page.component.html',
  styleUrls: ['./wallet-page.component.scss']
})
export class WalletPage {
  public wallet = '';

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://localhost:5000/wallet')
      .then(response => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then(data => {
        this.wallet = JSON.stringify(data);
        console.log('API Response:', data);
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }
}

