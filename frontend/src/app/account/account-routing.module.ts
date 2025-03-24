import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { WalletPage } from './pages/wallet/wallet-page.component';

const routes: Routes = [
  { path: 'wallet', component: WalletPage },
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class AccountRoutingModule {}